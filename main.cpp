#include <stdio.h>
#include <tgbot/tgbot.h>


constexpr const char* TOKEN = "6628082920:AAGagMYG5ObFde3Y1DusFmf4rGm0M4au9Qo";
constexpr int USER_ID = 710890997;



void sendTextMessage(TgBot::Bot &bot, const std::string &message);

void sendPhotoMessage(TgBot::Bot &bot, const std::string &path, const std::string &mime = "");

void sendDocumentMessage(TgBot::Bot &bot, const std::string &path, const std::string &mime = "");

void sendMultyDefaultMessage(TgBot::Bot &bot);


void catchPhotoMessage(TgBot::Bot &bot);



void printUser(TgBot::Message::Ptr &&message);

template <typename T>
void listen(TgBot::Bot &bot, const T &handler);



int main() 
{
    TgBot::Bot bot(TOKEN);

    //sendPhotoMessage(bot, "media/cat.jpg");
    //sendDocumentMessage(bot, "media/text.txt");
    //sendDocumentMessage(bot, "media/word.docx");
    //sendMultyDefaultMessage(bot);                       // some problems... in documentation! 2 - 3 words about it blyat`
    //listen(bot, printUser);
    catchPhotoMessage(bot);
}




void sendTextMessage(TgBot::Bot &bot, const std::string &message)
{
    try {
        bot.getApi().sendMessage(USER_ID, message);
    }
    catch (const std::exception &exc) {
        std::cerr << "Catched exception:\t" << exc.what() << std::endl;
    }
}

void sendPhotoMessage(TgBot::Bot &bot, const std::string &path, const std::string &mime)
{
    try {
        bot.getApi().sendPhoto(USER_ID, TgBot::InputFile::fromFile(path, mime));
    }
    catch (const std::exception &exc) {
        std::cerr << "Catched exception:\t" << exc.what() << std::endl;
    }
}

void sendDocumentMessage(TgBot::Bot &bot, const std::string &path, const std::string &mime)
{
    try {
        bot.getApi().sendDocument(USER_ID, TgBot::InputFile::fromFile(path, mime));
    }
    catch (const std::exception &exc) {
        std::cerr << "Catched exception:\t" << exc.what() << std::endl;
    }
}

void sendMultyDefaultMessage(TgBot::Bot &bot)
{
    std::vector<TgBot::InputMedia::Ptr> media_files;

    auto                                                                    // create file object
        file1 = TgBot::InputFile::fromFile("media/cat.jpg", "image/jpeg"),
        file2 = TgBot::InputFile::fromFile("media/rect.bmp", "image/bmp"),
        file3 = TgBot::InputFile::fromFile("media/rect.png", "image/png");

    auto                                                                    // send files, and get message object
        mess1 = bot.getApi().sendPhoto(USER_ID, file1),
        mess2 = bot.getApi().sendPhoto(USER_ID, file2),
        mess3 = bot.getApi().sendPhoto(USER_ID, file3);

    const std::string                                                       // remembering sended file id (or use method api.getFile)
        file_id1 = mess1->photo.back()->fileId,
        file_id2 = mess2->photo.back()->fileId,
        file_id3 = mess3->photo.back()->fileId;


    TgBot::InputMedia::Ptr                                                  // create input media object
        media1(new TgBot::InputMedia),
        media2(new TgBot::InputMedia),
        media3(new TgBot::InputMedia);

    media1->type = "photo"; media1->media = file_id1;                       // initialize type and file id (where constructors??? where exceptions??? WTF?????)
    media2->type = "photo"; media2->media = file_id2;
    media3->type = "photo"; media3->media = file_id3;


    media_files.push_back(media1);                                          // add media data
    media_files.push_back(media2);
    media_files.push_back(media3);
      
    bot.getApi().sendMediaGroup(USER_ID, media_files);                      // sending...
}

void catchPhotoMessage(TgBot::Bot &bot)
{
    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr &&message) {
       
        if (message->photo.size()) {                                                                                // <-- catcher photos
            const auto &metrix = message->photo;
            TgBot::PhotoSize::Ptr
                photo1 = metrix.back(),                                                                             // best quality
                photo2 = metrix.front(),                                                                            // worst quality
                photo3 = metrix[metrix.size() / 2];                                                                 // middle quality


            std::string data1 = bot.getApi().downloadFile(bot.getApi().getFile(photo1->fileId)->filePath);          // remembering that string can use how byites!
            std::string data2 = bot.getApi().downloadFile(bot.getApi().getFile(photo2->fileId)->filePath);
            std::string data3 = bot.getApi().downloadFile(bot.getApi().getFile(photo3->fileId)->filePath);

            std::ofstream                                                                                           // open how binary
                ph1("media/sas-best-img1.jpg", std::ios::binary),
                ph2("media/sas-worst-img2.jpg", std::ios::binary),
                ph3("media/sas-middle-img3.jpg", std::ios::binary);
            ph1 << data1;                                                                                           // write byites
            ph2 << data2; 
            ph3 << data3;
            ph1.close();  
            ph2.close();  
            ph3.close();

            std::cout << "Download photos\n";
        }
        else if (message->document) {
            TgBot::File::Ptr file = bot.getApi().getFile(message->document->fileId);                                // create file object
            std::string data = bot.getApi().downloadFile(file->filePath);                                           // get byites
            std::ofstream f("media/sas-doc-" + message->document->fileName, std::ios::binary);                      // use binary writing "Bloude documentation be silent about it"
            f << data;
            f.close();

            std::cout << "Download document\n"; 
        }
    });

    TgBot::TgLongPoll poll(bot);

    try {
        while (true) {
            poll.start();
        }
    }
    catch (const std::exception &exc) {
        std::cerr << "Catched exception:\t" << exc.what() << std::endl;
    }
}




void printUser(TgBot::Message::Ptr &&message)
{
    std::cout <<
        "Catching message:...................................................................................................\n" <<
        "First name: \t" << (message->from->firstName.empty() ? "\'None\'" : message->from->firstName) << '\n' <<
        "Second name:\t" << (message->from->lastName.empty() ? "\'None\'" : message->from->lastName) << '\n' <<
        "User id:    \t" << message->from->username << '\n' <<
        "Message:    \t" << message->text <<
        "\n....................................................................................................................\n";
}



template <typename T>
void listen(TgBot::Bot &bot, const T &handler)
{
    bot.getEvents().onAnyMessage(handler);
    TgBot::TgLongPoll poll(bot);

    try {
        while (true) {
            poll.start();
        }
    }
    catch (const std::exception &exc) {
        std::cerr << "Catched exception:\t" << exc.what() << std::endl;
    }
}
