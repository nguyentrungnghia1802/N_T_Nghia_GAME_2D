(Include 2 languages: English and Vietnamese)

----------------------------------------------------------------------------------------------------------------------------------------------------------

***GUIDE***

1, Building the file:
All file build and app running operations are done through the Terminal.
If you don't have Terminal, press "Ctrl + `" to call it.
BUILD FILE WITH THE FOLLOWING COMMAND:

g++ -Isrc/include -Lsrc/lib -o main src/main.cpp src/CommonFunc.cpp src/BaseObject.cpp src/gamemap.cpp src/MainObject.cpp src/ImpTimer.cpp src/BulletObject.cpp src/ThreatObject.cpp src/PlayHealth.cpp src/TextObject.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

Run the file with the command: ./main

<Remember to "Ctrl + S" before building the file>

2, Game Introduction: "T-Kun's Journey"
(Fantasy world)
In the mystical world of the Jurassic era, when the earth was still covered in dense forests and valleys, there is a touching story of love and bravery.
The little dinosaur girl named Isha is mischievous and adorable, always dreaming of grand adventures.
The son of the forest deity, named T-Rex, fell in love with Isha from the first moment he laid eyes on her.
And it seems Isha also secretly loves T-Rex, often exploring everything with him, calling him by the endearing name: T-Kun.
They are two souls meant to harmonize with each other, but fate leads them into a dangerous trial.

One day, while Maya was wandering through the forest alone, she got trapped in a trap set by a fearsome creature named DarkClaw.
DarkClaw is a ferocious and brutal monster, always hunting and capturing other creatures for food.
When T-Rex learns of this, he cannot sit idly by. He decides to leave his forest realm and embark on a journey to rescue Isha.
The power of love and courage leads T-Rex through dangerous challenges and past the awaiting Threats.

The road ahead is full of obstacles and challenges...
Please help T-Kun find Isha soon by assisting him in overcoming valleys, mazes, and defeating the Threats.
The day they reunite is not far away...
Don't give up!

3, Game Instructions:
Starting the game, replaying, and quitting the game:
In the MENU section: + Press SPACE to start playing
                     + Click the "X" button at the top right of the window to exit the game
While in the game: + Click the "X" button at the top right of the window to exit the game
                   + When the character dies 4 times, the game pauses and shows the score. Press SPACE to replay.
When "beating the game": + T-Kun and Isha reunite, showing the score and the number of days passed.
                         + Press SPACE to replay.
                        + Click the "X" button at the top right of the window to exit the game
Actions in the game: + Key "d": move right 
                     + Key "a": move left
                     + Key "w": jump
                     + Left mouse button: shoot bullets
Game Content: + You must continuously control T-Kun to move right to avoid being caught by the DarkClaw monster.
              + If T-Kun is caught (touches the left edge of the screen), falls into a ravine, or collides with Threats on the way, one life will be deducted.
              + Collect hearts along the way to earn points.
              + Use bullets to eliminate Threats blocking the path.
              + Flexibly use movement keys to overcome valleys, mazes.
              + When reaching the end of the journey and meeting Isha, the game will end and show the score.

**NOTE: The "a" key is not useless as you might think, it's essential



----------------------------------------------------------------------------------------------------------------------------------------------------------

***HƯỚNG DẪN***

1, Cách build file:
Các thao tác build, chạy app đều thao tác bằng Terminal.
Nếu không có Terminal bấm "Ctrl + `" để gọi.

BUILD FILE BẰNG LỆNH SAU: 
g++ -Isrc/include -Lsrc/lib -o main src/main.cpp src/CommonFunc.cpp src/BaseObject.cpp src/gamemap.cpp src/MainObject.cpp src/ImpTimer.cpp src/BulletObject.cpp src/ThreatObject.cpp src/PlayHealth.cpp src/TextObject.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

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

+ Ở mục MENU: + Nhấn chọn START để bắt đầu chơi
              + Nhấn chọn EXIT để thoát game

+ Khi vào game: + Nhấn nút "X" ở góc phải window để thoát game
                + Khi nhân vật chết 4 mạng, game dừng và hiện điểm. Nhấn phím SPACE để chơi lại, nhấn chọn dấu "X" ở góc trên bên phải để thoát game.
                + Khi hiện thông báo Journey: Nhấn phím SPACE để tiếp tục.

+ Khi "phá đảo": + T-Kun và Isha được đoàn tụ, hiện điểm, số ngày trải qua. 
                 + Nhấn phím SPACE để chơi lại.
                 + Nhấn chọn dấu "X" ở góc trên bên phải để thoát game.

- Các thao tác trong game:  + Phím "d": di chuyển sang phải
                            + Phím "a": di chuyển sang trái
                            + Phím "w": nhảy lên
                            + Chuột trái: bắn đạn

- Nội dung Game:  + Bạn phải liên tục điều khiển T-Kun di chuyển sang phải để tránh bị quái vật DarkClaw bắt được.
                  + Nếu T-Kun bị quái vật bắt(chạm vào rìa trái màn hình), rơi xuống vực hoặc va chạm với Threats trên hành trình sẽ bị trừ một mạng.
                  + Nhặt trái tim trên đường đi để nhận điểm.
                  + Sử dụng đạn để tiêu diệt Threats cản đường.
                  + Sử dụng linh hoạt các phím di chuyển để vượt qua thung lũng, mê cung.
                  + Khi đi hết hành trình và gặp được Isha thì game sẽ kết thúc và hiện điểm.

**NOTE: Phím "a" không vô dụng như bạn nghĩ đâu, nó rất cần thiết đấy

----------------------------------------------------------------------------------------------------------------------------------------------------------





