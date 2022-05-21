### Webserver

```c++
    Webserver srv(8080/* 监听的端口号 */);

    // 处理get请求方法
	svr.get("/test", [](HttpRequest &req, HttpResponse &res) {
		res.set_body("Hello World");
		res.set_code(200);
	});

    // 处理post请求方法
	svr.post("/insert_data", [](HttpRequest &req, HttpResponse &res) {
	});


	// 进行请求分发
	svr.dispatch();
```