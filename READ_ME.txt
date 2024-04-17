***HƯỚNG DẪN***

1, Cách build file:
Các thao tác build, chạy app đều thao tác bằng Terminal.
Nếu không có Terminal bấm "Ctrl + `" để gọi.

BUILD FILE BẰNG LỆNH SAU: 
g++ -Isrc/include -Lsrc/lib -o main main.cpp CommonFunc.cpp BaseObject.cpp gamemap.cpp MainObject.cpp ImpTimer.cpp BulletObject.cpp  ThreatObject.cpp PlayHealth.cpp TextObject.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

Chạy file bằng lệnh: ./main

<Nhớ "Ctrl + S" trước khi build file>

2, Giới thiệu Game: "T-Kun's Journey"
(Thế giới giả tưởng)

Trong thế giới huyền bí của kỷ Jura, khi mặt đất vẫn trải đầy rừng rậm và thung lũng, có một câu chuyện cảm động về tình yêu và sự dũng cảm.
Cô bé khủng long tên là Isha, là một cô bé tinh nghịch và đáng yêu, luôn mơ mộng về những cuộc phiêu lưu xa xôi. 
Cậu con trai của vị thần rừng, tên là T-Rex, đã yêu Isha từ lần đầu tiên anh nhìn thấy cô bé.
Và dường như Isha cũng thầm yêu T-Rex, thường đi khám phá mọi thứ cùng cậu, và gọi cậu với cái tên thân thương: T-Kun.
Họ là hai linh hồn sinh ra để tương hợp với nhau, nhưng vận mệnh lại đưa họ vào cuộc thử thách đầy nguy hiểm.

Một ngày, khi Maya đang thăm rừng một mình, cô bị mắc kẹt trong một cái bẫy do một con quái vật đáng sợ tên là DarkClaw tạo ra. 
DarkClaw là một con quái vật hung dữ và tàn bạo, luôn săn đuổi và bắt giữ những sinh vật khác để ăn thịt.
Khi T-Rex biết về việc này, anh không thể ngồi yên. Anh quyết định rời khỏi vùng rừng của mình và bắt đầu hành trình giải cứu Isha. 
Sức mạnh của tình yêu và lòng dũng cảm đã dẫn dắt T-Rex đi qua những thử thách nguy hiểm và vượt qua những con quái thú Threats đang chờ đợi.

Chặng đường phía trước đầy chông gai và thử thách... 
Bạn hãy giúp T-Kun sớm tìm được Isha bằng cách giúp cậu ấy vượt qua những thung lũng, mê cung, tiêu diệt các Threats.
Ngày mà cả hai đoàn tụ sẽ không xa...
Đừng bỏ cuộc nhé!

3, Hướng dẫn chơi game:
- Khởi động game, chơi lại và thoát game:
+ Ở mục MENU: nhấn phím SPACE để bắt đầu chơi
              nhấn phím ESC để thoát game

+ Khi vào game: Nhấn nút "X" ở góc phải window để thoát game
                Khi nhân vật chết 4 lần, game dừng và hiện điểm. Nhấn phím SPACE để chơi lại.

+ Khi "phá đảo": T-Kun và Isha được đoàn tụ, hiện điểm, số ngày trải qua. 
                 Nhấn phím SPACE để chơi lại.
                 Nhấn phím ESC để thoát game.

+ Các thao tác trong game:  Phím "d": di chuyển sang phải
                            Phím "a": di chuyển sang trái
                            Phím "w": nhảy lên
                            Chuột trái: bắn đạn

   