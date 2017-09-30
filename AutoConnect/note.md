# Note

## addParameter

add các tham số khác như mqtt_server,mqtt_port trước khi autoConnect.


## Autosave

Wifi manager sẽ tự động lưu SSID và  password lần gần nhất để conneect tới nên sẽ không thể tùy chình. Để không lưu gia trị đó thêm dòng `wifiManager.resetSettings();` trước khi autoConnect.