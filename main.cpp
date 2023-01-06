#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <vector>
#include <stack>
#include <termios.h>
#include <unistd.h>
#include <filesystem>
#include <fcntl.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <ctime>
#include<signal.h>

#define clearscr() printf("\033[H\033[J") // x1b[2J in hexadecimal
#define alternatebuffer() printf("\033[?1049h")
#define disable() printf("\033[=7l")
#define Red() printf("\033[0;31m");
#define White() printf("\033[0;39m");
#define Green() printf("\033[0;32m");
#define Blue() printf("\033[0;34m");
#define bold() printf("\u001b[1m");
#define normal() printf("\033[0m")

using namespace std;
char pwd[1024];
char currdir[1024];
char *homedir;
int xpos;
int ypos;
int terminalrows_no;
int terminalcols_no;
int curr_win_size;
struct termios initialsettings, newsettings;
struct winsize terminal;
vector<string> dirfileinfo;
stack<string> gobackward;
stack<string> goforward;
void move_cursor();
string completePath(string path);
void display_info(const char *directory);
void formDirInfo();
void enableNormalMode();
void fetchterminal();
void filesAndDirList(char *path);
void backSpace();
void enter();
void move_left();
void move_right();
void disableNormalmode();
bool create_dir(string path);
bool create_file(string path);
void move(string source, string dest);
bool goto_func(string path);
void copydir(string source, string dest);

void SetNormalMode();
void copyfile(string source, string dest);
bool create_dir(string path);
void filecopy(string source, string dest);
void copy_DirOrFile(string source, string dest);
void filecopy(string source, string dest);
void copyfile(string source, string dest);
void copy_DirOrFile(string source, string dest);
void delete_file(string path);
void delete_dir(string path);
bool search(string path, string dest);
void Commandmode();
void SetNormalMode();

void move_cursor()
{
    printf("\033[%d;%dH", xpos, ypos);
}
string completePath(string path)
{
    string abspath = "";
    if (path[0] == '~')
    {
        abspath = string(pwd) + path.substr(1, path.length());
    }
    else if (path[0] == '.' && path[1] != '/')
    {
        abspath = currdir;
    }
    else if (path[0] == '.' && path[1] == '/')
    {
        abspath = string(currdir) + path.substr(1, path.length());
    }
    else
    {
        abspath = string(currdir) + "/" + path;
    }

    return abspath;
}
void display_info(const char *directory)
{
    // ypos = 0;

    struct stat st;
    string abspath = completePath(directory);
    stat(abspath.c_str(), &st);
    long long size = st.st_size;
    char dim = 'B';
    // printf("\t");
    printf(S_ISDIR(st.st_mode) ? "d" : "-");
    printf((st.st_mode & S_IRUSR) ? "r" : "-");
    printf((st.st_mode & S_IWUSR) ? "w" : "-");
    printf((st.st_mode & S_IXUSR) ? "x" : "-");
    printf((st.st_mode & S_IRGRP) ? "r" : "-");
    printf((st.st_mode & S_IWGRP) ? "w" : "-");
    printf((st.st_mode & S_IXGRP) ? "x" : "-");
    printf((st.st_mode & S_IROTH) ? "r" : "-");
    printf((st.st_mode & S_IWOTH) ? "w" : "-");
    printf((st.st_mode & S_IXOTH) ? "x" : "-");
    ypos = ypos + 11;
    int count = 0;
    while (size >= 1024)
    {
        size = size / 1024;
        count++;
    }
    if (count == 0)
    {
        dim = 'B';
    }
    else if (count == 1)
    {
        dim = 'K';
    }
    else if (count == 2)
    {
        dim = 'M';
    }
    else if (count == 3)
    {
        dim = 'G';
    }
    ypos = ypos + printf("%5lld%c\t", size, dim);
    struct passwd *pw = getpwuid(st.st_uid);
    struct group *gr = getgrgid(st.st_gid);
    ypos = ypos + printf("%s\t", pw->pw_name);
    ypos = ypos + printf("%s\t", gr->gr_name);
    char mtime[80];
    time_t t = st.st_mtime; /*st_mtime is type time_t */
    struct tm lt;
    localtime_r(&t, &lt); /* convert to struct tm */
    strftime(mtime, sizeof mtime, "%d-%b-%Y %H:%M", &lt);
    ypos = ypos + printf("%s\t", mtime);
    ypos = ypos + printf("%s\n", directory);
    ypos++;
}
void formDirInfo()
{
    clearscr();
    int show;
    xpos = 1;
    ypos = 1;
    move_cursor();
    // printf("extra\nextra\n");
    
    if (dirfileinfo.size() <= terminalrows_no)
        show = dirfileinfo.size() - 1;
    else
        show = terminalrows_no - 2 + curr_win_size;
    // printf("%d\n",curr_win_size);
    for (int i = curr_win_size; i <= show; i++)
    {
        // display(directory_list[i].c_str());
        // printf("%d ",i);
        display_info(dirfileinfo[i].c_str());
        // xpos++;
    }
    /* xpos=1;
     ypos=1;
     move_cursor();*/
    /*if(dirfileinfo.size()<terminalrows_no){
     xpos=terminalrows_no+4;
     ypos=1;
     move_cursor();
      printf("NORMAL MODE: %s\n",currdir);
    }*/
    /*else{
     //terminalrows_no;
     xpos=terminalrows_no+2;
    }*/
    // printf("%d\t%d\t%d%lu\t",xpos,terminalrows_no,curr_win_size,dirfileinfo.size());
  //  xpos = dirfileinfo.size() + 2;
   // xpos=1;
    xpos=terminalrows_no+3;
    ypos = 1;
    move_cursor();
    // printf("%d\t%d\t%d%lu\t",xpos,terminalrows_no,curr_win_size,dirfileinfo.size());
    Blue();
    bold();
    printf(">>>>>>>   NORMAL MODE: %s", currdir);

    White();

    xpos = 1;
    ypos = 1;
    move_cursor();
    // printf("xpos:%d\typos:%d",xpos,ypos);
    return;
}
void enableNormalMode()
{
    tcgetattr(STDIN_FILENO, &initialsettings);
    newsettings = initialsettings;
    newsettings.c_lflag &= ~ICANON;
    newsettings.c_lflag &= ~ECHO;
    //newsettings.c_lflag&= ~
    tcsetattr(STDIN_FILENO, TCSANOW, &newsettings);

    return;
}
void fetchterminal()
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal);
    terminalrows_no = terminal.ws_row - 3;
    terminalcols_no = terminal.ws_col +1000;
}

void filesAndDirList(char *path)
{
    fetchterminal();
    clearscr();
    DIR *dirpath;
    struct dirent *dir;
    dirpath = opendir(path);
    dirfileinfo.clear();
    while ((dir = readdir(dirpath)))
    {
        // cout<<"dir name:"<<dir->d_name<<endl;
        //printf("%c adding files", dir->d_type);
       // ;
        dirfileinfo.push_back(dir->d_name);
    }
    sort(dirfileinfo.begin(), dirfileinfo.end());
    formDirInfo();
    xpos = 1;
    move_cursor();
    closedir(dirpath);
    return;
}
void backSpace()
{

    clearscr();
    // xpos=1;
    // ypos=1;
    // move_cursor();
    // printf("yes here it came!!!!!!");
    string prevdir = string(currdir).substr(0, string(currdir).find_last_of("\\/"));
    // printf("%s\n",prevdir.c_str());
    if (strcmp(homedir, currdir) == 0)
    {
        filesAndDirList(currdir);
        return;
    }
    //  printf("%s",homedir);
    strcpy(currdir, prevdir.c_str());
    gobackward.push(currdir);
    while (!goforward.empty())
    {
        goforward.pop();
    }
    // printf("%s\n",currdir);
    // printf("%s\n",prevdir.c_str());
    filesAndDirList(currdir);

    return;
}
void enter()
{
    if (dirfileinfo[curr_win_size + xpos - 1] == ".")
    {
        xpos = 1;
        move_cursor();
        filesAndDirList(currdir);
    }
    else if (dirfileinfo[curr_win_size + xpos - 1] == "..")
    {
        backSpace();
        gobackward.push(currdir);
        while (!goforward.empty())
            goforward.pop();
    }
    else
    {

        string path = string(currdir) + "/" + dirfileinfo[curr_win_size + xpos - 1];
        // printf("%s",path.c_str());
        struct stat st;
        stat(path.c_str(), &st);
        while (!goforward.empty())
            goforward.pop();
        if (S_ISDIR(st.st_mode))
        {

            clearscr();

            // printf("%d\t%d\t",xpos,ypos);
            /*  xpos=1;
              ypos=1;
              move_cursor();
            //printf("ITS an directory\n");*/
            strcpy(currdir, path.c_str());
            gobackward.push(currdir);
            // printf("%s",currdir);
            curr_win_size = 0;
            filesAndDirList(currdir);
            // printf("%d",curr_win_size);
            xpos = 1;
            ypos = 1;
            move_cursor();
        }
        else
        {
            //  printf("ITS an file\n");
            pid_t pid = fork();
            //  printf("%d\n",pid);
            if (pid == 0)
            {
                // printf("yes fork call");
                close(2);
                execlp("xdg-open", "xdg-open", path.c_str(), NULL);
                exit(0);
            }
            else
                filesAndDirList(currdir);
        }
    }
    // printf("xpos:%d",xpos);
    return;
}
void move_left()
{
    xpos = 1;
    ypos = 1;
    curr_win_size = 0;
    move_cursor();
    if (gobackward.size() == 1)
    {
        string path = gobackward.top();
        strcpy(currdir, path.c_str());
        filesAndDirList(currdir);
    }
    else if (gobackward.size() > 1)
    {
        string path = gobackward.top();
        gobackward.pop();
        goforward.push(path);
        path = gobackward.top();
        strcpy(currdir, path.c_str());
        filesAndDirList(currdir);
    }
}
void move_right()
{
    xpos = 1;
    ypos = 1;
    curr_win_size = 0;
    move_cursor();
    if (!goforward.empty())
    {
        string path = goforward.top();
        goforward.pop();
        strcpy(currdir, path.c_str());
        gobackward.push(currdir);
        filesAndDirList(currdir);
    }
}
void disableNormalmode()
{
    xpos = terminalrows_no + 3;
    ypos = 1;
    move_cursor();
    printf("\x1b[0K"); // remove normal mode text
    xpos = dirfileinfo.size() + 2;
    ypos = 1;
    move_cursor();
    printf("\x1b[0K");
    printf(":\n");
    // cout<<endl;
    fflush(0);
    ypos++;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &initialsettings);
    // exit(1)
    return;
}
bool create_dir(string path)
{
    int res = mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
    //cout << "create dir result" << res << "\n";
    if (res == 0)
        return true;
    else
        return false;
}
bool create_file(string path)
{
    int res = open(path.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    // printf("res%d",res);
    if (res >= 0)
        return true;
    else
        return false;
}
void move(string source, string dest)
{
    struct stat stsource, stdest;
    stat(source.c_str(), &stsource);
    stat(dest.c_str(), &stdest);

    if (S_ISDIR(stsource.st_mode))
    {
        if (S_ISDIR(stdest.st_mode))
        {
            string sourcedir = source.substr(source.find_last_of('/'));
            // ifcreate dir;
            // {
            //   // cout<<"moving "<<source<<" "<<dest<<"\n";
            // change dest
            //cout << "moving " << source << " " << dest << "\n";
            copy_DirOrFile(source, dest);
            delete_dir(source);
           // printf("moved successfully!!\n");
            // }
            // else
            //{
            //   printf("error\n");
            // }
            // copy content
            // delete source dir
        }
    }
    else
    {
       
        if (S_ISDIR(stdest.st_mode))
        {
             // cout<<dest + "/" + sourcefile<<"\n";
            // copy file
            copyfile(source, dest);
            delete_file(source);
           // printf("moved successfully!!\n");
        }
        else
        { 
            Red();
            printf("Error\n");
            White();
        }

        // delete file
    }
}

bool goto_func(string path)
{
    string cmpath = completePath(path);
    strcpy(currdir, path.c_str());
    int res = chdir(path.c_str());
    if (res == 0)
        return true;
    return false;
}
void copydir(string source, string dest)
{
    struct stat stsource, stdest;
    stat(source.c_str(), &stsource);
    stat(dest.c_str(), &stdest);
    //cout << "moving: " << source << "into " << dest << "\n";
    if (S_ISDIR(stsource.st_mode))
    {
        if (S_ISDIR(stdest.st_mode))
        {
           // cout << "here directory copy\n";
            DIR *dir;
            struct dirent *inDir;
            struct stat st;
            dir = opendir(source.c_str());
            chdir(source.c_str());
        //    cout << "checkpoint 0\n";
        //    cout << dir << "dir\n";
            while ((inDir = readdir(dir)) != NULL)
            {
               // cout << "checkpoint 1\n";
                lstat(inDir->d_name, &st);
                if (S_ISDIR(st.st_mode))
                {
                    // cout<<inDir->d_name<<"\n";
                   // cout << "checkpoint 2\n";
                    if (strcmp(".", inDir->d_name) == 0 || strcmp("..", inDir->d_name) == 0)
                    {
                        // cout<<"inside if "<<inDir->d_name<<"\n";
                        continue;
                    }
                   // cout << "makedir: " << (dest + "/" + inDir->d_name) << "\n";
                    mkdir((dest + "/" + inDir->d_name).c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
                    copydir(inDir->d_name, dest + "/" + inDir->d_name);
                }
                else
                {
                    //cout << "copy file:" << inDir->d_name << "\t" << dest << "\n";
                    copyfile(inDir->d_name, dest);
                }
            }
            chdir("..");
            closedir(dir);
        }
    }
}
void filecopy(string source, string dest)
{
    string sourcename = source.substr(source.find_last_of('/') + 1);
    string destinationfilename = dest + "/" + sourcename;
   // cout << destinationfilename << " file to copy here\n";
    int dread;
    char data[2048];
    int s = open(source.c_str(), O_RDONLY);
    int d = open(destinationfilename.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    while ((dread = read(s, data, sizeof(data))))
    {
        write(d, data, dread);
    }
    return;
}
void copyfile(string source, string dest)
{
    // cout<<"copy file:"<<source<<"\t"<<dest<<"\n";

    string destinationfilename = dest + "/" + source;
    int dread;
    char data[2048];
    int s = open(source.c_str(), O_RDONLY);
    int d = open(destinationfilename.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    while ((dread = read(s, data, sizeof(data))))
    {
        write(d, data, dread);
    }
    return;
}
void copy_DirOrFile(string source, string dest)
{
    struct stat stsource, stdest;
    stat(source.c_str(), &stsource);
    stat(dest.c_str(), &stdest);

    if (S_ISDIR(stsource.st_mode))
    {
        if (S_ISDIR(stdest.st_mode))
        {
            // cout<<source<<"\n";
            // cout<<dest<<"\n";
            // string abssource=completePath(source);
            // string absdest=completePath(dest);
            string sourcename = source.substr(source.find_last_of('/') + 1);
            // cout<<sourcename<<"\n";
            dest = dest + "/" + sourcename;
            // cout<<"creating "<<dest<<"\n";
            if (create_dir(dest))
            {
                // cout<<"created sourcefolder to dest";
                // dest=dest+"/"+sourcename;
                // cout<<dest;
                copydir(source, dest);
            }
        }
    }
    else
    {
        if (S_ISDIR(stdest.st_mode))
        {
            // printf("here file copy:%s");
            // string absdest=completePath(dest);
            // cout<<absdest<<endl;
            // cout<<"copy file???"<<source<<"\t"<<dest<<"\n";
            // string sourcename=source.substr(source.find_last_of('/')+1);
            copyfile(source, dest);
        }
    }
}
void delete_file(string path)
{
    unlink(path.c_str());
    return;
}
void delete_dir(string path)
{
    DIR *dir;
    struct dirent *indir;

    if ((dir = opendir(path.c_str())) == NULL)
    {
        fprintf(stderr, "Can't open the directory: %s\n", path.c_str());
        return;
    }

    while ((indir = readdir(dir)) != NULL)
    {
        if (strcmp(indir->d_name, ".") == 0 || strcmp(indir->d_name, "..") == 0)
            continue;
        else
        {
            string pathdelete = path + "/" + string(indir->d_name);
            struct stat st;
            stat(pathdelete.c_str(), &st);
            if (S_ISDIR(st.st_mode))
                delete_dir(pathdelete);
            else
                unlink(pathdelete.c_str());
        }
    }
    remove(path.c_str());
    closedir(dir);
}
bool search(string path, string dest)
{
    // string dest=currdir;
    // cout<<dest<<"\n";
    struct stat st;
    stat(dest.c_str(), &st);
    bool flag = false;
    if (S_ISDIR(st.st_mode))
    { // check whether destination passed is a directory
        DIR *dir;
        // cout<<"yes its a directory";
        struct dirent *indir;
        struct stat st;
        dir = opendir(dest.c_str());
        chdir(dest.c_str());
        while ((indir = readdir(dir)) != NULL)
        { // read one by one form dest
            lstat(indir->d_name, &st);
            if (S_ISDIR(st.st_mode))
            { // If destination is directory
                if (strcmp(".", indir->d_name) == 0 || strcmp("..", indir->d_name) == 0)
                {
                    continue;
                }
                if (indir->d_name == path)
                {
                    flag = true;
                    break;
                }
                dest = dest + "/" + indir->d_name;
                flag = search(path, dest);
                if (flag)
                    break;
            }
            else
            {
                if (indir->d_name == path)
                {
                    flag = true;
                    break;
                }
            }
        }
        chdir("..");
        closedir(dir);
    }
    return flag;
}
void Commandmode()
{
    disableNormalmode();
    string command = "";
    string tokencommand[10];
     enableNormalMode();
    
    // scanf("%s",str);
    // cin>>str;
    // cout<<str;

    // cout<<str;
    // printf("%s",str.c_str());
    
    while (true)
    { command="";
        char ch;
       
        
        while(((ch = cin.get())!= 27)){
        if(ch==127)
			{
                //cout<<"backspace\n";
                printf("\33[2K\r");
				if(command.length()<=1)
				{
					command="";
				}
				else{
					command = command.substr(0,command.length()-1);
				}
				cout<<command;
				
			}
            else if(ch==10){
                command+='\0';
                cout<<"\n";
                break;

            }
			else{
				command = command + ch;
				cout<<ch;	
			}
       }
       // printf("command %s",command.c_str());
        // cin>>command;
        vector<string> tokencommand;
        //getline(cin, command);
        /* if(c=='\033'){
             clearscr();
             // SetNormalMode();
             filesAndDirList(currdir);
             SetNormalMode();
             return;
         }*/
         //cout<<"command is :"<<command<<"\n";
        if (ch== '\033')
        {

            clearscr();
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &initialsettings);
           // exit(1);
           // SetNormalMode();
           //cout<<"out of command mode\n";
           filesAndDirList(currdir);
           SetNormalMode();
           break;
           //cout<<"hello command";
          //return;
            
        }
        else
        {
            int start = 0, i = 0, j = 0;
            while (command[i] != '\0')
            {
                if (command[i] == ' ')
                {
                    tokencommand.push_back(command.substr(start, i - start));
                    // printf("start:%d\t i:%d",start,i);
                    start = i + 1;
                    // printf("%s\n",tokencommand[j].c_str());
                    j++;
                }
                i++;
            }
            tokencommand.push_back(command.substr(start, i - start));
            if (tokencommand[0] == "copy")
            {
                // printf("copy\n");
                if (tokencommand.size() == 3)
                {
                    struct stat stsource, stdest;
                    stat(tokencommand[1].c_str(), &stsource);
                    stat(tokencommand[2].c_str(), &stdest);
                    int index = tokencommand[1].find_first_of('/');
                    // cout<<index<<"index of /\n";

                    if (S_ISDIR(stsource.st_mode) == 0 && index != -1)
                    {
                       // cout << "called filecopy\n";
                        filecopy(tokencommand[1], tokencommand[2]);
                        Green();
                        printf("Copied successfully!!\n");
                        White();
                    }
                    else
                    {
                        copy_DirOrFile(tokencommand[1], tokencommand[2]);
                        Green();
                        printf("Copied Successfully\n");
                        White();
                    }
                }

                else
                {
                    Red();
                    printf("Syntax Error!!\n");
                    White();
                }
            }

            else if (tokencommand[0] == "move")
            {
                // printf("move\n");
                move(tokencommand[1], tokencommand[2]);
                Green();
                    printf("Move successfully!!\n");
                    White();
            }
            else if (tokencommand[0] == "delete_file")
            {
                // printf("delete file\n");
                delete_file(tokencommand[1]);
                Green();
                    printf("Deleted successfully!!\n");
                    White();
            }
            else if (tokencommand[0] == "delete_dir")
            {
                // printf("delete dir\n");
                delete_dir(tokencommand[1]);
                Green();
                    printf("Deleted successfully!!\n");
                    White();

            }
            else if (tokencommand[0] == "rename")
            {
                // printf("rename\n");
                string oldname = tokencommand[1];
                string newname = tokencommand[2];
                if (rename(oldname.c_str(), newname.c_str()) == 0)
                {
                    Green();
                    printf("Renamed successfully!!\n");
                    White();
                }
                else
                {
                    Red();
                    printf("Error!!");
                    White();
                }
            }
            else if (tokencommand[0] == "create_file")
            {
                // printf("create_file\n");
                if (tokencommand.size() >= 3)
                {
                    if (tokencommand[2].substr(0, 1) == "~")
                    {
                        tokencommand[2] = homedir + tokencommand[2].substr(1);
                        // tokencommand[2]=homedir;
                    }
                    string path = tokencommand[2] + "/" + tokencommand[1];
                    if (!create_file(path)){
                        Red();
                        printf("Error while creating file\n");
                        White();
                    }
                    else
                    {
                        Green();
                        printf("File created successfully!!\n");
                        White();
                    }
                }

                else
                {
                    Red();
                    printf("Check the syntax\n");
                    White();
                }
            }
            else if (tokencommand[0] == "create_dir")
            {
                // printf("create_dir\n");
                if (tokencommand.size() >= 3)
                {
                    if (tokencommand[2].substr(0, 1) == "~")
                    {
                        tokencommand[2] = homedir + tokencommand[2].substr(1);
                        // tokencommand[2]=homedir;
                    }
                    string path = tokencommand[2] + "/" + tokencommand[1];
                    if (!create_dir(path))
                    { Red();

                        printf("Error while creating directory\n");
                         White();
                    }
                    else
                    {
                        Green();
                        printf("Directory created successfully!!\n");
                        White();
                    }
                }

                else
                {
                    Red();
                    printf("Check the syntax\n");
                    White();
                }
            }
            else if (tokencommand[0] == "goto")
            {
                // printf("goto\n");
                if (goto_func(tokencommand[1]))
                {
                    Green();
                    printf("Switch to normal mode to see the changed current directory\n");
                    White();
                }
                else
                {
                    Red();
                    printf("Error!!");
                    White();
                }
            }
            else if (tokencommand[0] == "search")
            {
                // printf("search\n");
                if (search(tokencommand[1], currdir))
                {
                    Green();
                    printf("True\n");
                    White();
                }
                else
                {
                    Red();
                    printf("False\n");
                    White();
                }
            }
            else
            {
                Red();
                printf("Command not found\n");
                White();
            }
        }

        // read(str);
        // printf("%s",read.c_)
    }
   // cout<<"out\n";
  // return;
}
void SetNormalMode()
{
    char input[3];
    enableNormalMode();
    //printf("\x1b[=7l");
    while (true)
    {
        // printf("%d\t%d",xpos,ypos);

        // break;
        // xpos=0;
        // printf("%d xpos",xpos);
        move_cursor();

        fflush(0);
        read(STDIN_FILENO, input, 3);
        // printf("input :%c%c%c",input[0],input[1],input[2]);
        // break;

        if (input[0] == 'q')
        {
           
            clearscr();
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &initialsettings);
            //tcsetattr(STDIN_FILENO, TCSAFLUSH, &initialsettings);
        // cout<<"out of normal mode\n";
            xpos = 1;
            ypos = 1;
            move_cursor();
          // break;
            //fflush(0);
            //return;
            exit(1);
            
        }
        else if (input[0] == ':')
        {
            // command mode switch code
            // clearscr();
            // xpos=1;
            // ypos=1;
            // move_cursor();
            // filesAndDirList(currdir);
            Commandmode();
          //  cout<<"back to\n";
           // return;

            exit(1);
        }
        else if (input[0] == 10)
        {
            // Enter Key
            // printf("ENTER KEY PRESSED\n");
            clearscr();
            enter();
        }
        else if (input[0] == 'h')
        {
            // home dir
            // printf("home directory\n");
            strcpy(currdir, homedir);
            while (!goforward.empty())
                goforward.pop();
            filesAndDirList(homedir);
        }
        else if (input[0] == 127)
        {
            // backspace i.e go up one level
            clearscr();
            //  printf("backspace key pressed\n");
            backSpace();
            // exit(1);
        }
        else if (input[0] == '\033' && input[1] == '[' && input[2] == 'A')
        {

            // Scroll up
            // printf("scroll up\n");
            if (xpos > 1)
            {
                xpos--;
                move_cursor();
            }
            else if (xpos == 1 && xpos + curr_win_size > 1)
            {
                curr_win_size--;
                formDirInfo();
            }
        }
        else if (input[0] == '\033' && input[1] == '[' && input[2] == 'B')
        {
            // Scroll Down
            // printf("%d\t",xpos);
            if (xpos <= terminalrows_no && xpos < dirfileinfo.size())
            {
               int count=xpos;
               xpos++;
                move_cursor();
               // clearscr();
               // formDirInfo();
            }
            else if (xpos > terminalrows_no && xpos + curr_win_size < dirfileinfo.size() - 1)
            {
                curr_win_size++;
                formDirInfo();
            }
        }
        else if (input[0] == '\033' && input[1] == '[' && input[2] == 'C')
        {
            // Scroll right
            // printf("scroll right\n");
            xpos = 1;
            ypos = 1;
            move_cursor();
            curr_win_size = 0;
            move_right();
        }
        else if (input[0] == '\033' && input[1] == '[' && input[2] == 'D')
        {
            // Scroll left
            // printf("scroll left\n");
            xpos = 1;
            ypos = 1;
            move_cursor();
            curr_win_size = 0;
            move_left();
        }

        // if()
    }
}
void winsz_handler(int sig) {
    clearscr();
    xpos = 1;
    ypos = 1;
    move_cursor();
    fetchterminal();
    
    
    formDirInfo();
    
}
int main()
{
    signal(SIGWINCH, winsz_handler);

    clearscr();
    getcwd(pwd, 1024);
    getcwd(currdir, 1024);

    if ((homedir = getenv("HOME")) == NULL)
    {
        homedir = getpwuid(getuid())->pw_dir;
    }

    alternatebuffer(); // if buffer required
    gobackward.push(pwd);
    filesAndDirList(pwd);
    // printf("%d\t%d",xpos,ypos);
    //printf("\033[=7l");
    SetNormalMode();
    exit(0);
}
