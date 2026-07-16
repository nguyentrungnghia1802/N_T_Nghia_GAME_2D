# Game Refactoring Plan

## 1. Tổng quan hiện trạng

- Kiến trúc hiện tại: game SDL2 C++ nhỏ, phần runtime nằm chủ yếu trong `src/main.cpp`, `MainObject`, `ThreatsObject`, `BulletObject`, `GameMap`, `BaseObject`, `TextObject`, `PlayerPower` và `PlayerMoney`. Nhiều trạng thái game và tài nguyên SDL đang là biến global trong `main.cpp` và `CommonFunc.h`.
- Luồng game loop: `main.cpp:136-397` chạy `while (!is_quit)`, poll event, clear renderer, update background/map/player/threat/bullet/HUD, `SDL_RenderPresent`, rồi tự cap 60 FPS bằng `ImpTimer` và `SDL_Delay`.
- Cách quản lý entity: player là object global; threats được tạo bằng `MakeThreats()` trả về `std::vector<ThreatsObject*>`; bullet được tạo bằng `new BulletObject()` khi click chuột và giữ trong vector raw pointer của `MainObject`.
- Cách quản lý tài nguyên: ảnh dùng `BaseObject::LoadImg()` để `IMG_Load` rồi `SDL_CreateTextureFromSurface`; font, sound, menu/win/journey surface/texture nằm trong biến global. Chưa có resource cache chung.
- Cách sử dụng thread: không phát hiện `std::thread`, SDL thread hoặc async task trong runtime. `main.cpp` có include `<chrono>` và `<thread>` nhưng không tạo thread.
- Nguyên nhân chính có khả năng gây lag hoặc tốn RAM: reload texture trong update/render, leak ownership của threat arrays, cleanup không đầy đủ, update/render mọi threat mỗi frame, collision bullet-threat dạng nested loop, tạo texture text mỗi frame, các `SDL_Delay(1000/4000)` và modal loop chặn game loop.

## 2. Các vấn đề nghiêm trọng phát hiện được

### [Critical] Reload texture trong lúc update/render

- File liên quan: `src/MainObject.cpp`, `src/ThreatObject.cpp`, `src/BaseObject.cpp`
- Class/function liên quan: `MainObject::Show`, `ThreatsObject::ImpMoveType`, `BaseObject::LoadImg`
- Hiện trạng: player gọi `LoadImg()` mỗi lần `Show()` dựa trên hướng đi; threat động gọi `LoadImg()` khi đổi hướng hoặc khi chưa chạm đất.
- Tác động: gây I/O, tạo/hủy texture GPU, spike CPU/GPU và stutter trong frame gameplay.
- Bằng chứng trong code: `MainObject.cpp:93-102`; `ThreatObject.cpp:274-323`; `BaseObject.cpp:17-39` gọi `Free()`, `IMG_Load`, `SDL_CreateTextureFromSurface`.
- Hướng xử lý ngắn gọn: preload texture theo trạng thái/hướng, giữ pointer/cache, chỉ đổi reference texture khi state đổi.

### [Critical] Leak bộ nhớ threat do `new[]` nhưng không có owner/delete[]

- File liên quan: `src/main.cpp`
- Class/function liên quan: `MakeThreats`, restart/win cleanup trong `main`
- Hiện trạng: `MakeThreats()` cấp phát 4 mảng `new ThreatsObject[NUM_THREATS_LIST]`, push pointer từng phần tử vào vector, nhưng code chỉ gọi `Free()` texture rồi `clear()` vector; không giữ pointer gốc để `delete[]`.
- Tác động: RAM tăng sau restart/win/replay; ownership không rõ, dễ dangling pointer nếu xóa từng phần tử.
- Bằng chứng trong code: cấp phát tại `main.cpp:819`, `835`, `856`, `877`; cleanup chỉ `Free()` một threat rồi `break` tại `main.cpp:141-150` và `main.cpp:295-304`.
- Hướng xử lý ngắn gọn: chuyển threat storage sang `std::vector<std::unique_ptr<ThreatsObject>>` hoặc vector object ổn định, không dùng `new[]` rời rạc.

### [Critical] Shutdown và cleanup SDL chưa đầy đủ

- File liên quan: `src/main.cpp`, `src/TextObject.cpp`
- Class/function liên quan: `main`, `close`, `LoadFromFile`, `Create_texture`, `TextObject::~TextObject`
- Hiện trạng: khi vòng game kết thúc, `main` `return 0` mà không gọi `close()`; `close()` không giải phóng `TTF_Font`, win/journey surface/texture, `TextObject` texture, `TTF_Quit`; `TextObject` destructor trống.
- Tác động: leak font/texture/surface/sound khi thoát bình thường hoặc khi quay lại nhiều lần; khó kiểm tra leak bằng tool.
- Bằng chứng trong code: `main.cpp:397-398`; tài nguyên load ở `main.cpp:403-428` và `main.cpp:738-754`; cleanup thiếu các nhóm đó tại `main.cpp:431-465`; `TextObject.cpp:12-14`.
- Hướng xử lý ngắn gọn: gom shutdown qua một đường duy nhất, dùng RAII deleter cho SDL resource, destructor `TextObject` gọi `Free()`, đóng font bằng `TTF_CloseFont`.

### [High] Threat được update/render toàn bộ mỗi frame, chưa có activation/sleep

- File liên quan: `src/main.cpp`, `src/ThreatObject.cpp`
- Class/function liên quan: game loop threat section, `ThreatsObject::DoPlayer`, `ThreatsObject::Show`
- Hiện trạng: mỗi frame duyệt toàn bộ `threats_list`, gọi move/update/show/collision dù threat ở xa camera.
- Tác động: CPU và draw call tăng theo số threat toàn map; animation/physics của object ngoài vùng chơi vẫn chạy.
- Bằng chứng trong code: `main.cpp:206-225`; `ThreatObject.cpp:70-110`.
- Hướng xử lý ngắn gọn: thêm active range theo player/camera; threat ngoài vùng active chỉ giữ state tối thiểu, không AI/physics/animation/render.

### [High] Collision bullet-threat là nested loop trên danh sách hiện tại

- File liên quan: `src/main.cpp`, `src/CommonFunc.cpp`
- Class/function liên quan: bullet collision block, `SDLCommonFunc::CheckCollision`
- Hiện trạng: mỗi bullet kiểm tra với mọi threat đang trong vector.
- Tác động: độ phức tạp `O(num_bullets * num_threats)`; sẽ spike khi nhiều bullet/threat.
- Bằng chứng trong code: vòng lặp lồng nhau tại `main.cpp:313-346`; collision thủ công tại `CommonFunc.cpp:10-43`.
- Hướng xử lý ngắn gọn: chỉ collision các entity active/visible trước; nếu số entity tăng, dùng spatial grid/chunk query.

### [High] Game loop chưa có deltaTime, có blocking delay và modal loop

- File liên quan: `src/main.cpp`, `src/ImpTimer.cpp`
- Class/function liên quan: main game loop, menu/game-over/win/journey loops
- Hiện trạng: movement/animation dùng hằng số mỗi frame; FPS cap dùng `SDL_Delay`; có `SDL_Delay(1000)` khi mất mạng và `SDL_Delay(4000)` khi start/replay; menu/win/journey có loop riêng.
- Tác động: frame-time spike, input bị chặn, gameplay phụ thuộc FPS, khó chuyển sang Emscripten main loop callback.
- Bằng chứng trong code: `main.cpp:156`, `245`, `267`, `385-395`, `572-663`, `669-697`, `766-809`; `ImpTimer.cpp:22-60`.
- Hướng xử lý ngắn gọn: tạo `deltaTime` có clamp, đổi màn hình menu/win/journey thành state trong main loop, thay delay dài bằng timer/state transition.

### [Medium] Text HUD tạo texture mới mỗi frame

- File liên quan: `src/main.cpp`, `src/TextObject.cpp`
- Class/function liên quan: `time_game`, `heart_game`, `high_score_game`, `TextObject::LoadFromRenderText`
- Hiện trạng: timer, score, high score gọi `LoadFromRenderText()` mỗi frame; hàm này `Free()` texture cũ rồi render surface/texture mới.
- Tác động: allocation liên tục, CPU spike từ TTF render và texture upload.
- Bằng chứng trong code: `main.cpp:363-383`; `TextObject.cpp:16-29`.
- Hướng xử lý ngắn gọn: chỉ rebuild text texture khi text thay đổi; timer có thể cập nhật theo giây, score theo event.

### [Medium] Map/tile resource bị reload khi restart

- File liên quan: `src/main.cpp`, `src/gamemap.cpp`
- Class/function liên quan: `Restart`, `GameMap::LoadMap_Return`, `GameMap::LoadTiles`
- Hiện trạng: restart gọi lại load map file và load tile texture.
- Tác động: I/O và texture churn trong lúc replay; với map lớn hơn sẽ tạo spike rõ.
- Bằng chứng trong code: `main.cpp:700-704`; `gamemap.cpp:50-60`, `123-170`.
- Hướng xử lý ngắn gọn: load tile một lần; giữ map template trong memory rồi reset map state từ bản sao sạch.

### [Medium] Phụ thuộc Windows API cản trở WebAssembly

- File liên quan: `src/CommonFunc.h`, `src/main.cpp`
- Class/function liên quan: include global, limited-time message
- Hiện trạng: include `windows.h` trong header chung và dùng `MessageBoxW`.
- Tác động: không build trực tiếp bằng Emscripten; `CommonFunc.h` kéo Windows API vào nhiều translation unit.
- Bằng chứng trong code: `CommonFunc.h:4`; `main.cpp:353-359`.
- Hướng xử lý ngắn gọn: thay bằng SDL message/state UI hoặc abstraction platform nhỏ; tránh native API trong header runtime.

## 3. Kế hoạch refactor

### Bước 1: Thêm đo lường và profiling cơ bản

- Mục tiêu: có số liệu trước/sau cho frame time, update/render count, collision count, texture/font/sound count, RAM/thread.
- Vấn đề được giải quyết: thiếu baseline để biết thay đổi nào thật sự cải thiện.
- File dự kiến chỉnh sửa: `src/main.cpp`, có thể thêm module nhỏ `Profiler`.
- Nội dung cần làm: đếm frame time bằng SDL performance counter, số entity update/render, số collision check, số texture/font/sound load; log định kỳ hoặc overlay debug.
- Điều không được thay đổi: gameplay, tốc độ di chuyển, collision result.
- Cách kiểm tra sau khi hoàn thành: chạy 3-5 phút, ghi min/avg/max FPS/frame time và kiểm tra log ổn định.
- Rủi ro: log quá nhiều gây nhiễu hiệu năng; cần throttle.
- Phụ thuộc vào bước: không có.

### Bước 2: Sửa ownership và cleanup tài nguyên

- Mục tiêu: loại leak rõ ràng trước khi tối ưu sâu.
- Vấn đề được giải quyết: leak threat arrays, font/surface/texture, shutdown không thống nhất.
- File dự kiến chỉnh sửa: `src/main.cpp`, `src/TextObject.cpp`, `src/TextObject.h`, các object giữ SDL resource.
- Nội dung cần làm: dùng một đường shutdown duy nhất; `TextObject::~TextObject` gọi `Free()`; `TTF_CloseFont` cho mọi font; destroy/free win/journey texture/surface; đổi threat ownership khỏi `new[]` raw pointer.
- Điều không được thay đổi: số threat, vị trí spawn, luật restart/win.
- Cách kiểm tra sau khi hoàn thành: chạy, restart nhiều lần, thoát bằng window close/ESC/menu; RAM không tăng đều, không double-free.
- Rủi ro: chuyển ownership sai có thể làm invalid pointer trong collision/update.
- Phụ thuộc vào bước: Bước 1 khuyến nghị.

### Bước 3: Cache/preload texture, sound và font

- Mục tiêu: bỏ load trùng và load trong frame.
- Vấn đề được giải quyết: `LoadImg()` trong `Show()`/`ImpMoveType`, texture bullet/threat/player bị tạo lại nhiều lần.
- File dự kiến chỉnh sửa: `src/BaseObject.*`, `src/MainObject.*`, `src/ThreatObject.*`, `src/BulletObject.*`, `src/main.cpp`, `src/gamemap.cpp`.
- Nội dung cần làm: preload player left/right, threat direction textures, bullet texture, tile texture; object chỉ giữ reference/handle; sound/font load một lần.
- Điều không được thay đổi: sprite sheet, frame clip, animation frame order.
- Cách kiểm tra sau khi hoàn thành: log không còn `IMG_Load` sau khi gameplay bắt đầu; FPS/frame-time spike giảm.
- Rủi ro: lifetime texture phải dài hơn entity dùng nó.
- Phụ thuộc vào bước: Bước 2.

### Bước 4: Chuẩn hóa game loop với deltaTime và state machine

- Mục tiêu: giảm blocking spike và chuẩn bị cho Emscripten.
- Vấn đề được giải quyết: movement phụ thuộc FPS, `SDL_Delay` dài, menu/win/journey loop riêng.
- File dự kiến chỉnh sửa: `src/main.cpp`, `src/ImpTimer.*`, entity update signatures.
- Nội dung cần làm: tính `deltaTime` có clamp; thay delay 1s/4s bằng timer state; gom menu/gameplay/gameover/win/journey vào một loop.
- Điều không được thay đổi: cảm giác tốc độ hiện tại; khi quy đổi dt phải giữ tương đương 60 FPS.
- Cách kiểm tra sau khi hoàn thành: gameplay ở 60 FPS giống trước, pause/replay không đóng băng event processing.
- Rủi ro: dt sai làm thay đổi jump/gravity/threat speed.
- Phụ thuộc vào bước: Bước 1.

### Bước 5: Thêm entity activation/sleeping

- Mục tiêu: không update AI/physics/animation threat quá xa player/camera.
- Vấn đề được giải quyết: toàn bộ threat update mỗi frame.
- File dự kiến chỉnh sửa: `src/main.cpp`, `src/ThreatObject.*`, có thể thêm helper camera/activation.
- Nội dung cần làm: xác định camera rect từ `map_data.start_x_`; chỉ active threat trong camera mở rộng hoặc gần player; threat inactive không gọi `ImpMoveType`, `DoPlayer`, `Show`, collision.
- Điều không được thay đổi: threat phải hoạt động đúng khi người chơi tới gần.
- Cách kiểm tra sau khi hoàn thành: counter entity update/render giảm khi player ở đầu map; threat gần vẫn di chuyển/collision đúng.
- Rủi ro: inactive quá hẹp làm threat xuất hiện bất ngờ hoặc sai vị trí.
- Phụ thuộc vào bước: Bước 1, Bước 4 khuyến nghị.

### Bước 6: Thêm camera culling cho entity/effect

- Mục tiêu: không render object ngoài camera.
- Vấn đề được giải quyết: `ThreatsObject::Show` và bullet render chưa check viewport.
- File dự kiến chỉnh sửa: `src/main.cpp`, `src/ThreatObject.*`, `src/MainObject.cpp`, `src/BulletObject.*`, `src/BaseObject.*`.
- Nội dung cần làm: thêm helper intersect với camera/screen rect; chỉ gọi render nếu visible. Tile map đã culling theo viewport trong `GameMap::DrawMap`.
- Điều không được thay đổi: background scrolling và HUD luôn render.
- Cách kiểm tra sau khi hoàn thành: render count giảm; không mất object khi vào camera.
- Rủi ro: rect/frame size hiện có sai có thể cull nhầm.
- Phụ thuộc vào bước: Bước 5 khuyến nghị.

### Bước 7: Tối ưu collision bằng active list/spatial query

- Mục tiêu: giảm collision check không cần thiết.
- Vấn đề được giải quyết: bullet-threat nested loop và player-threat check toàn danh sách.
- File dự kiến chỉnh sửa: `src/main.cpp`, `src/CommonFunc.*`, `src/ThreatObject.*`, `src/BulletObject.*`.
- Nội dung cần làm: dùng active/visible threat list trước; sau đó nếu entity tăng, dùng uniform grid theo TILE_SIZE/chunk để query threat gần bullet/player.
- Điều không được thay đổi: hitbox gameplay hiện tại nếu chưa có quyết định chỉnh collision.
- Cách kiểm tra sau khi hoàn thành: collision count/frame giảm, kết quả hit vẫn giống trước trong test tình huống.
- Rủi ro: `CheckCollision` hiện hard-code size 115/95 và 150/100; sửa hitbox có thể đổi gameplay nên cần tách riêng.
- Phụ thuộc vào bước: Bước 5.

### Bước 8: Giảm allocation runtime

- Mục tiêu: giảm cấp phát liên tục trong frame và lúc bắn.
- Vấn đề được giải quyết: bullet `new/delete`, text texture mỗi frame, copy vector bullet.
- File dự kiến chỉnh sửa: `src/MainObject.*`, `src/BulletObject.*`, `src/TextObject.*`, `src/main.cpp`.
- Nội dung cần làm: bullet pool hoặc vector object tái sử dụng; text dirty flag; `get_bullet_list()` trả const reference hoặc cung cấp iterator để tránh copy.
- Điều không được thay đổi: fire rate, hướng đạn, score display.
- Cách kiểm tra sau khi hoàn thành: allocation count giảm, không crash khi bắn liên tục.
- Rủi ro: pool lifecycle sai có thể tái dùng bullet đang active.
- Phụ thuộc vào bước: Bước 2, Bước 3.

### Bước 9: Chuẩn hóa map reset và chunk metadata

- Mục tiêu: giảm I/O/reload khi restart và mở đường cho spatial partitioning.
- Vấn đề được giải quyết: reload map/tile khi restart, thiếu chunk/region cho activation.
- File dự kiến chỉnh sửa: `src/gamemap.*`, `src/main.cpp`.
- Nội dung cần làm: load tile một lần; giữ `base_map` và `runtime_map`; tạo helper query tile range theo camera/chunk; reset bằng copy map data sạch.
- Điều không được thay đổi: tile IDs, heart collection, checkpoint map_start.
- Cách kiểm tra sau khi hoàn thành: restart không gọi lại `LoadTiles`; map/heart reset đúng.
- Rủi ro: copy map sai có thể làm heart đã ăn không reset hoặc reset quá sớm.
- Phụ thuộc vào bước: Bước 3.

### Bước 10: Tách phụ thuộc platform để chuẩn bị WebAssembly

- Mục tiêu: source runtime build được theo hướng SDL2/Emscripten.
- Vấn đề được giải quyết: `windows.h`, `MessageBoxW`, loop dạng blocking, global platform state.
- File dự kiến chỉnh sửa: `src/CommonFunc.h`, `src/main.cpp`, build file.
- Nội dung cần làm: thay `MessageBoxW` bằng SDL/UI state; bỏ Windows include khỏi header chung; chuẩn bị hàm `UpdateFrame()` có thể gọi từ `emscripten_set_main_loop`.
- Điều không được thay đổi: nội dung thông báo và flow thua/thắng.
- Cách kiểm tra sau khi hoàn thành: build native vẫn chạy; không còn dependency trực tiếp Windows trong source runtime trừ build config cần thiết.
- Rủi ro: tách loop quá rộng có thể chạm nhiều file, nên làm sau khi đã ổn định gameplay.
- Phụ thuộc vào bước: Bước 4.

## 4. Thứ tự ưu tiên đề xuất

1. Bước 1 trước vì cần baseline để chứng minh refactor có ích.
2. Bước 2 ngay sau đó vì leak/ownership là lỗi tài nguyên nghiêm trọng nhất.
3. Bước 3 để bỏ nguyên nhân spike lớn nhất: load texture trong frame.
4. Bước 4 để game loop không còn blocking và chuẩn bị Emscripten.
5. Bước 5 và Bước 6 để giảm update/render object ngoài vùng chơi.
6. Bước 7 để collision scale tốt hơn sau khi đã có active list.
7. Bước 8 để giảm allocation runtime còn lại.
8. Bước 9 để map/reset sạch hơn và hỗ trợ chunk/spatial về sau.
9. Bước 10 cuối cùng vì là cải thiện kiến trúc/portability, ít cấp bách hơn leak và spike runtime.

## 5. Chỉ số cần đo trước và sau refactor

- FPS trung bình.
- FPS thấp nhất.
- Frame time trung bình.
- Frame time cao nhất.
- CPU usage.
- RAM usage.
- Số thread.
- Số entity được update mỗi frame.
- Số entity được render mỗi frame.
- Số collision check mỗi frame.
- Số texture, sound và font đang được load.
- Số lần gọi `IMG_Load`, `TTF_RenderText_*`, `SDL_CreateTextureFromSurface` trong gameplay.
- RAM có tăng theo thời gian hay không, đặc biệt sau restart/win/replay và khi bắn liên tục.

## 6. Các câu hỏi hoặc điểm chưa thể xác nhận

- Chưa có số đo runtime thực tế nên chưa thể định lượng chính xác CPU/RAM/frame spike hiện tại.
- Chưa xác nhận intended behavior của threat ngoài camera: có cần tiếp tục di chuyển theo thời gian thật hay chỉ cần kích hoạt khi player tới gần.
- Chưa xác nhận asset size và VRAM thực tế vì không đọc ảnh/âm thanh lớn theo yêu cầu.
- Chưa xác nhận target Emscripten cụ thể, flags build và cách đóng gói asset cho web.
- Chưa xác nhận hitbox hard-code trong `SDLCommonFunc::CheckCollision` là cố ý để gameplay dễ hơn hay là bug cần sửa riêng.
