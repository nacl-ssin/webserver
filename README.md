### Webserver

> Reactor模型+线程池的WebServer

```c++
Webserver srv(8080/* 监听的端口号 */, false/*是否是ET模式,默认true*/);

svr.static_path(/*静态资源的访问路径*/);

// 处理get请求方法
svr.get("/test", [](HttpRequest &req, HttpResponse &res) {
    res.set_body("Hello World");
    res.set_code(200);
});

// 处理post请求方法
svr.post("/insert_data", [](HttpRequest &req, HttpResponse &res) {
});


// 进行请求分发
svr.start();
```

1. 增加线程池。
2. 增加了io多路复用技术。
3. 增加接收和发送缓冲区。
4. 增加Reactor模型对事件进行分发。