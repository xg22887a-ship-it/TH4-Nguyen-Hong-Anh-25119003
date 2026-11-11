# Nguyễn Hồng Anh - 25119003 - Thực hành 4

Mã nguồn và minh chứng được sắp xếp trong thư mục tương ứng của từng bài.

---

# Nguyễn Hồng Anh - 25119003 - Thực hành 4 - Thí nghiệm 2

Minh chứng, videos và code ở trong các folders tương ứng của mỗi bài !

# TH4 · REST/MQTT, Node-RED và dashboard thời gian thực — SV 25119003

**Phần cứng:** ESP32 + module AHT20 (0x38, nhiệt/ẩm) + BMP280 (0x77, áp suất) — SDA → GPIO21, SCL → GPIO22, VCC → 3V3. LED → GPIO5 (D5) qua điện trở 220 Ω *(dùng GPIO5 thay cho GPIO14 trong slide)*. **IP ESP32:** `192.168.0.101` · **Broker:** `test.mosquitto.org:1883` · **SV_ID:** `25119003`

## Bài 1 · REST: GET /api/sensor trả JSON

Route `GET /api/sensor` trả `application/json`, số đo thật từ AHT20/BMP280. Trường tự thêm: `id`, `nhom`, `pres_hPa`, `rssi`, `ip`.

```json
{"id":"25119003","nhom":"nhom3","temp":25.5,"hum":78.6,"pres_hPa":1019.9,"uptime":11,"rssi":-24,"ip":"192.168.0.101"}
```

## Bài 2 · MQTT: publish telemetry theo nhóm

Topic **`aet2022/25119003/telemetry`**, mỗi 2 s publish JSON; client id `esp32-25119003-<MAC>`. Trường tự thêm: `id`, `pres_hPa`, `rssi`.

```json
{"id":"25119003","temp":25.3,"hum":79.1,"pres_hPa":1019.8,"uptime":15,"rssi":-18}
```

## Bài 3 · Node-RED: dashboard realtime và chiều đi–về

- Chiều đi: ESP32 publish `aet2022/25119003/telemetry` → dashboard gauge Nhiệt độ, gauge Độ ẩm, chart nhiệt/ẩm.
- Chiều về: nút **BẬT/TẮT LED** publish `1`/`0` vào `aet2022/25119003/cmd/led` → ESP32 bật/tắt LED GPIO5.
- Tự thêm: **đèn LED ảo** (sáng/tắt theo trạng thái thật ESP32 gửi lên) và **thanh sóng Wi-Fi** (RSSI 4 cột).
- Flow: `bai_3/code/flow.json`.

## Bài 4 · Kiểm tra bằng MQTT client (MQTTX)

Không nạp firmware mới (dùng lại firmware Bài 3). Subscribe `aet2022/25119003/#`:

- `aet2022/25119003/telemetry` — JSON đúng định dạng, có các trường đã thêm.
- `aet2022/25119003/cmd/led` — xuất hiện `1` / `0` khi bấm nút trên dashboard.
- Subscribe `aet2022/#`: dữ liệu của mình chỉ nằm dưới `aet2022/25119003/`, không lẫn sang namespace nhóm khác nhờ SV_ID riêng.

## Mã nguồn

`bai_1/code` · `bai_2/code` · `bai_3/code` (+ `flow.json`) · `bai_4/code` — tên/mật khẩu Wi-Fi để trống (`TEN_WIFI` / `MAT_KHAU`).
