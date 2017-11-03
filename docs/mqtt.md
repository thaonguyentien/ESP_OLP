# MQTT

## 1. Giới thiệu về MQTT

MQTT là một giao thức vẩn chuyển tin nhắn thông qua cơ chế Pub/Sub. Nó là mã nguồn mở, đơn giản, và được thiết kế để có thể dễ dàng triển khai trong thực tế. Dó các đặt tính đó nên MQTT có thể được sử dụng rộng rãi trong nhiều tính huống như việc trao đổi giữa hai thiết bị máy tính và trong IOT.

## 2. Pub and Sub

Mô hình pub/sub được sử dung thay thế cho mô hình client/server truyền thống. Pub/sub cho phép phân tách(decopuling) giữa người nhận và người gửi. Có nghĩa là người gửi và người nhận không cần biết về sự tồn tại của người kia. Có một bên thứ 3 gọi là _broker_ là thành phần sẽ nắm giữ thông tin của cả người gửi(pub) và người nhận(sub), _broker_ sẽ lọc tất cả các gói tin tới nó và chuyển tới các người nhận(sub) phù hợp.

![https://www.hivemq.com/wp-content/uploads/pub-sub-mqtt-1024x588.png](https://www.hivemq.com/wp-content/uploads/pub-sub-mqtt-1024x588.png)

Cơ chế pub/sub cho phép decoupling theo 3 yếu tố:
- Space decoupling: Pub và Sub không cần biết thông tin về người kia( địa chỉ IP và port)
- Time decoupling: Pub và Sub không cần cùng chạy tại một thời điểm
- Synchronization decoupling: Các hoạt động khác trên các thành phần không bị dừng trong khi pub/sub.

### Scalability

Pub/sub cung cấp tính scalability lớn hơn so nhiều so với mô hình client/server truyền thống. Bởi vì việc thực hiện trên broker có thể thực hiện một cách song song và sử lý theo sự kiện. Việc lưu chữ(caching) tin nhắn và định tuyến thông minh cũng làm tăng tính scalability.

## MQTT

MQTT là một giao thức cụ thể theo mô hình pub/sub nhưng nó có những đặc tính riêng của nó. Nó có thể thực hiện đầy đủ các khía cạnh đã đề cập theo mô hình pub/sub hoặc cắt giảm bớt một số tính chất tùy vào cách bạn muốn.
- MQTT cho phép space decoupling giữa pub và sub. Do đó chỉ cần biết ip và port của broker để tiến hành pub và sub message.