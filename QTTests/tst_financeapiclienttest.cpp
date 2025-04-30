#include <QtTest>
#include "financeapiclient.h"

class MockRequest : public Request {
    Q_OBJECT
public:
    struct Call {
        QString method;
        QUrl url;
        QByteArray body;
    };

    QVector<Call> calls;

    void onPostRequestWithBody(QUrl url, const QByteArray &body) override {
        calls.append({"POST", url, body});
    }

    void onGetRequestWithBody(QUrl url, const QByteArray &body) override {
        calls.append({"GET", url, body});
    }

    void emitHttpData(const QByteArray& data) {
        emit gotHttpData(data);
    }

    void emitHttpFinished() {
        emit httpFinished();
    }
};

class FinanceApiClientTest : public QObject {
    Q_OBJECT

private slots:
    void test_addUser_generatesCorrectRequest();

    void test_getUser_emits_userReceived_onSuccess();

    void test_handleHttpFinished_emitsError_onMalformedJson();

    void test_transferMoney_generatesCorrectRequest();
};



void FinanceApiClientTest::test_addUser_generatesCorrectRequest() {
    auto mock = new MockRequest();
    FinanceApiClient client("http://localhost", nullptr);
    delete client.m_request;  // delete real request
    client.m_request = mock;

    client.addUser("lvr", "123", 150.5, "USER");

    QCOMPARE(mock->calls.size(), 1);
    const auto& call = mock->calls.first();
    QCOMPARE(call.method, QString("POST"));
    QCOMPARE(call.url.toString(), QString("http://localhost/add_user"));

    QJsonDocument doc = QJsonDocument::fromJson(call.body);
    QJsonObject obj = doc.object();

    QCOMPARE(obj["name"].toString(), QString("lvr"));
    QCOMPARE(obj["password"].toString(), QString("123"));
    QCOMPARE(obj["balance"].toDouble(), 150.5);
    QCOMPARE(obj["role"].toString(), QString("USER"));
}



void FinanceApiClientTest::test_getUser_emits_userReceived_onSuccess() {
    auto mock = new MockRequest();
    FinanceApiClient client("http://localhost", nullptr);
    delete client.m_request;
    client.m_request = mock;

    QObject::connect(mock, &Request::gotHttpData, &client, &FinanceApiClient::handleHttpData);
    QObject::connect(mock, &Request::httpFinished, &client, &FinanceApiClient::handleHttpFinished);

    QSignalSpy spy(&client, &FinanceApiClient::userReceived);
    QSignalSpy errorSpy(&client, &FinanceApiClient::errorOccurred);

    client.getUser("lvr");

    QByteArray response = R"(
        {
            "status": "success",
            "user": {
                "name": "lvr",
                "password": "secret",
                "balance": "300",
                "role": "ADMIN"
            }
        }
    )";
    mock->emitHttpData(response);
    mock->emitHttpFinished();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);

    User user = qvariant_cast<User>(spy.takeFirst().at(0));
    QCOMPARE(user.name, QString("lvr"));
    QCOMPARE(user.password, QString("secret"));
    QCOMPARE(user.balance, 300);
    QCOMPARE(user.role, QString("ADMIN"));
}

void FinanceApiClientTest::test_handleHttpFinished_emitsError_onMalformedJson() {
    auto mock = new MockRequest();
    FinanceApiClient client("http://localhost", nullptr);
    delete client.m_request;
    client.m_request = mock;

    QObject::connect(mock, &Request::gotHttpData, &client, &FinanceApiClient::handleHttpData);
    QObject::connect(mock, &Request::httpFinished, &client, &FinanceApiClient::handleHttpFinished);

    QSignalSpy spy(&client, &FinanceApiClient::errorOccurred);

    QByteArray badData = "This is not JSON";
    mock->emitHttpData(badData);
    mock->emitHttpFinished();

    QCOMPARE(spy.count(), 1);
    QString error = spy.takeFirst().at(0).toString();
    QCOMPARE(error, QString("This is not JSON"));
}




void FinanceApiClientTest::test_transferMoney_generatesCorrectRequest() {
    auto mock = new MockRequest();
    FinanceApiClient client("http://localhost", nullptr);
    delete client.m_request;
    client.m_request = mock;

    client.transferMoney("lvr", "rlv", 100.0);

    QCOMPARE(mock->calls.size(), 1);
    const auto& call = mock->calls.first();
    QCOMPARE(call.url.toString(), QString("http://localhost/transfer"));

    QJsonDocument doc = QJsonDocument::fromJson(call.body);
    QJsonObject obj = doc.object();

    QCOMPARE(obj["from"].toString(), QString("lvr"));
    QCOMPARE(obj["to"].toString(), QString("rlv"));
    QCOMPARE(obj["amount"].toDouble(), 100.0);
}

QTEST_MAIN(FinanceApiClientTest)
#include "tst_financeapiclienttest.moc"
