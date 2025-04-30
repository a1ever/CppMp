#include <QApplication>

#include "loginwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setStyleSheet(R"(
        QWidget {
            background-color: #1e1e2f;
            color: #ffffff;
            font-family: 'Segoe UI';
            font-size: 14px;
        }
        QLabel {
            color: #dddddd;
            font-weight: bold;
        }
        QPushButton {
            background-color: #2e8b57;
            color: white;
            border-radius: 8px;
            padding: 8px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #3cb371;
        }
        QPushButton:pressed {
            background-color: #276644;
        }
        QLineEdit, QListWidget {
            background-color: #2a2a3d;
            color: white;
            border: 1px solid #3e3e5e;
            border-radius: 6px;
            padding: 4px;
        }
)");

    auto api = new FinanceApiClient("http://localhost:8080", nullptr);

    LoginWindow loginWindow(api);
    loginWindow.show();
    return app.exec();
}
