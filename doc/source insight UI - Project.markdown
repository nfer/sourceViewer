# source insight UI - Project 

---

There are four parts in Project Menu:

- Project Manage
- Project Content Manage
- Project Status
- Project Settings

in source insight software, all Project actions are separated with below group:

![image](https://raw.githubusercontent.com/nfer/sourceViewer/master/doc/image/source%20insight/project%20menu_1.png)

but in my options, "Rebuild Project..." action should also be include in the second group "Project Content Manage", as this action is much like "Synchronize Files..." action with "Force all files to be re-parsed".

## New Project..
"New Project..." action has a Shortcuts "Alt+Shift+N", and "N" in here means "New".

### Close current project if has
If you already open one project, then you MUST close it before create a new one. Thus, when you press "Alt+Shift+N" shortcuts, a alert dialog shows:

![image](https://raw.githubusercontent.com/nfer/sourceViewer/master/doc/image/source%20insight/new%20project_1.png)

and you MUST click "OK", otherwise you couldn't continue.

### Set project name and store path
The first step to create a new project is to set the "project name" and "project store path":

![image](https://raw.githubusercontent.com/nfer/sourceViewer/master/doc/image/source%20insight/new%20project_2.png)

The default project name is "Untitled Project", and the default project store path is "USER_DOCUMENTS\Source Insight\PROJECTNAME". **USER_DOCUMENTS** is easy understood, while **PROJECT** means "if you input another name, the recommended store path's last folder name would update to the new name automatically". Make the project name and store path folder name the same is a good habit.

If you don't like the RECOMMENDED store path, you can input the store path or click "Browse" or press "Alt+B" shortcuts to open a File Dialog to choose another path.

### New Project Settings
After set project name and store path, the next step is set configurations of this project:

![image](https://raw.githubusercontent.com/nfer/sourceViewer/master/doc/image/source%20insight/new%20project_3.png)

but actually, you only need to set "Project Source Directory" where is the root folder of this project source files. Others, the default choose works fine for most project. 
So, in my software, I merge "set Project Source Directory" to the fist step, set project Name, Store Path and Source Root Path.

### Add and Remove Project Files
The final step is to add files to current project, and the initial folder is "Project Source Directory" which is set in last step.

![image](https://raw.githubusercontent.com/nfer/sourceViewer/master/doc/image/source%20insight/new%20project_4.png)

You can add one file by double click the file, add a select folder recursively, or add all files/folders in current path(with recursively optional).
But if you want remove files from project file list, you can remove them with one file, a folder tree or all files in project list.

There are five parts in “Add and Remove Project Files” Dialog:

1. File Name Text Editor
2. Current Directory Label
3. File System Tree View
4. Current Directory Contents List View
5. Project File List View

![image](https://raw.githubusercontent.com/nfer/sourceViewer/master/doc/image/source%20insight/new%20project_4_0.png)

#### File Name Text Editor
The initial value of File Name is NULL, and cleared by below actions:

- ENTER key pressed
- Enter another directory

And there are two functions with this Text Editor:

- File Name Filter: e.g. when you input "abc", then files(and directories) start with "abc" would show, and the fist item is selected.
- Enter another hard disk partition: e.g. when you input "d:" and press ENTER, then the current direcoty would switch to the root of Disk D.

**Note 1** Why not show all hard disk partitions in "File System Tree View"
**Note 2** The File Name Filter should be more intelligent, e.g. "list" or "flm" should match "FileListModel.cpp"
**Note 3** If the input not match anything, and user still press ENTER key, clear current input would be a better choise, or show a alert to notice user.
