# source insight Module - Project 

标签（空格分隔）： module

---

There are four parts in Project Menu:

- Project Manage
- Project Content Manage
- Project Status
- Project Settings

in source insight software, all Project actions are seprated with below group:

![image](https://raw.githubusercontent.com/nfer/sourceViewer/master/doc/image/source%20insight/project%20menu_1.png)

but in my opition, "Rebuild Project..." action should also be include in the second group "Project Content Manage", as this action is much like "Synchronize Files..." action with "Force all files to be re-parsed".

## New Project..
"New Project..." action has a Shortcuts "Alt+Shift+N", and "N" in here means "New".

### Close current project if has
If you already open one project, then you MUST close it before create a new one. Thus, when you press "Alt+Shift+N" shortcuts, a alert dialog shows:

![image](https://raw.githubusercontent.com/nfer/sourceViewer/master/doc/image/source%20insight/new%20project_1.png)

and you MUST click "OK", otherwise you couldn't continue.

### Set project name and store path
The first step to create a new project is to set the "project name" and "project store path":

![image](https://raw.githubusercontent.com/nfer/sourceViewer/master/doc/image/source%20insight/new%20project_2.png)

The default project name is "Untitled Project", and the default project store path is "USER_DOCUMENTS\Source Insight\PROJECTNAME". **USER_DOCUMENTS** is easy understood, while **PROJECT** means "if you input another name, the recommanded store path's last folder name would update to the new name automatically". Make the project name and store path folder name the same is a good habit.

If you don't like the RECOMMANDED store path, you can input the store path or click "Browse" or press "Alt+B" shortcuts to open a FileDialog to choose another path.

