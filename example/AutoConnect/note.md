# Note

## addParameter

- add các tham số khác như mqtt_server,mqtt_port trước khi autoConnect.

- Sử dụng : 
    `WiFiManagerParameter custom_mqtt_server("server", "mqtt server", default_mqtt_server, 40);`
    Để tạo tham số về server của mqtt.

- Sử dụng: `wifiManager.addParameter(&custom_text)` để thêm giao diện cho than số đó vào giao diện web.

- Sau khi người dùng chỉnh sửa các tham số(như mqtt_server hoặc mqtt_port) và sau khi đã connect thành công vào wifi ta có thể sử dụng câu lệnh để đọc các giá trị được người dùng nhập vào `custom_mqtt_server.getValue()` để lấy giá trị được nhập vào và xử lý.

- Ngoài các tham số như trên có thể thêm các tham số khác do mình tùy chỉnh như:     `WiFiManagerParameter custom_test("port", "mqtt test", default_mqtt_port, 6);` sau đó add vào giao diện web :    `wifiManager.addParameter(&custom_test);`.
- Để hiểu rõ hơn chạy ví dụ :<https://github.com/NTT-TNN/ESP_OLP/blob/master/AutoConnect/AutoConnect.ino>.




## Autosave

Wifi manager sẽ tự động lưu SSID và  password lần gần nhất để conneect tới nên sẽ không thể tùy chình. Để không lưu gia trị đó thêm dòng `wifiManager.resetSettings();` trước khi autoConnect.