#include <QCoreApplication>
#include <QHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocalServer>
#include <QLocalSocket>
#include <QTimer>

#include <random>

#include "kadence/config.hpp"
#include "kadence/session_analyzer.hpp"
#include "kadence/time_utils.hpp"
#include "kadence/types.hpp"

namespace kadence {

class KadenceDaemon : public QObject {
public:
    explicit KadenceDaemon(QObject *parent = nullptr)
        : QObject(parent)
        , rng_(std::random_device{}())
        , frame_time_dist_(6.0f, 16.0f) {
        session_.id = 1;
        session_.process_id = 0;
        session_.process_name = "synthetic";

        synthetic_timer_.setInterval(16);
        QObject::connect(&synthetic_timer_, &QTimer::timeout, this, [this]() {
            generate_sample();
        });

        QObject::connect(&server_, &QLocalServer::newConnection, this, [this]() {
            handle_new_connection();
        });
    }

    bool start() {
        QLocalServer::removeServer(server_name_);
        if (!server_.listen(server_name_)) {
            return false;
        }
        synthetic_timer_.start();
        return true;
    }

private:
    void handle_new_connection() {
        while (QLocalSocket *socket = server_.nextPendingConnection()) {
            buffers_.insert(socket, QByteArray{});

            QObject::connect(socket, &QLocalSocket::readyRead, this, [this, socket]() {
                handle_ready_read(socket);
            });

            QObject::connect(socket, &QLocalSocket::disconnected, this, [this, socket]() {
                buffers_.remove(socket);
                socket->deleteLater();
            });
        }
    }

    void handle_ready_read(QLocalSocket *socket) {
        auto &buffer = buffers_[socket];
        buffer.append(socket->readAll());

        while (true) {
            const int newline_index = buffer.indexOf('\n');
            if (newline_index < 0) {
                break;
            }

            const QByteArray line = buffer.left(newline_index).trimmed();
            buffer.remove(0, newline_index + 1);

            if (line.isEmpty()) {
                continue;
            }

            handle_request(socket, line);
        }
    }

    void handle_request(QLocalSocket *socket, const QByteArray &line) {
        const QJsonDocument request_doc = QJsonDocument::fromJson(line);
        if (!request_doc.isObject()) {
            send_error(socket, 0, "invalid_json");
            return;
        }

        const QJsonObject request = request_doc.object();
        const int id = request.value("id").toInt(0);
        const QString type = request.value("type").toString();

        if (type == "get_live_metrics") {
            send_live_metrics(socket, id);
        } else {
            send_error(socket, id, "unknown_command");
        }
    }

    void send_live_metrics(QLocalSocket *socket, int id) {
        QJsonObject payload;
        payload.insert("id", id);
        payload.insert("type", "live_metrics");
        payload.insert("fps", session_.stats.avg_fps);
        payload.insert("frame_time_ms", last_frame_time_ms_);
        payload.insert("p1_low_fps", session_.stats.p1_low_fps);
        payload.insert("p01_low_fps", session_.stats.p01_low_fps);
        payload.insert("dropped_last_sec", static_cast<int>(session_.stats.dropped_last_sec));

        write_response(socket, payload);
    }

    void send_error(QLocalSocket *socket, int id, const QString &message) {
        QJsonObject payload;
        payload.insert("id", id);
        payload.insert("type", "error");
        payload.insert("message", message);
        write_response(socket, payload);
    }

    void write_response(QLocalSocket *socket, const QJsonObject &payload) {
        const QJsonDocument response_doc(payload);
        QByteArray response = response_doc.toJson(QJsonDocument::Compact);
        response.append('\n');
        socket->write(response);
        socket->flush();
    }

    void generate_sample() {
        FrameSample sample;
        sample.timestamp_ns = now_monotonic_ns();
        sample.frame_time_ms = frame_time_dist_(rng_);
        sample.dropped = sample.frame_time_ms >= DROPPED_FRAME_THRESHOLD_MS;

        last_frame_time_ms_ = sample.frame_time_ms;
        analyzer_.add_sample(session_, sample);
    }

    const QString server_name_ = QStringLiteral("kadence");
    QLocalServer server_;
    QTimer synthetic_timer_;
    QHash<QLocalSocket *, QByteArray> buffers_;

    Session session_;
    SessionAnalyzer analyzer_;
    std::mt19937 rng_;
    std::uniform_real_distribution<float> frame_time_dist_;
    double last_frame_time_ms_ = 0.0;
};

} // namespace kadence

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    kadence::KadenceDaemon daemon;
    if (!daemon.start()) {
        return 1;
    }

    return app.exec();
}
