#include <mysql/mysql.h>
#include "webserver.h"

MYSQL *get_connection() {
	MYSQL *conn = mysql_init(nullptr);
	mysql_set_character_set(conn, "utf8");
	mysql_real_connect(conn, "127.0.0.1", "root", "@Sing187817", "test", 3306, nullptr, 0);
	return conn;
}

int main() {
	Webserver svr(8080);


	svr.get("/test", [](HttpRequest &req, HttpResponse &res) {
		res.set_body("Hello World");
		res.set_code(200);
	});


	svr.post("/insert_data", [](HttpRequest &req, HttpResponse &res) {
		MYSQL *connect = get_connection();
		std::string username = req.get_params("username");
		std::string password = req.get_params("password");

		if (!username.empty() && !password.empty()) {
			std::string sql = "insert into user(username, password) values(\'" + username + "\', \'" + password + "\')";
			LOG_INFO("execute sql ： %s", sql.c_str());
			int ret = mysql_query(connect, sql.c_str());
			if (ret == 0) {
				res.set_body("insert data ok");
				res.set_code(200);
			} else {
				res.set_body("insert failed");
				res.set_code(200);
			}
		} else {
			LOG_WARRING("params is empty");
			res.set_body("params is empty");
			res.set_code(200);
		}

	});


	svr.dispatch();
	return 0;
}
