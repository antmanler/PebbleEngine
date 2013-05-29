
#include <PythonQt/PythonQt.h>
//#include <PythonQt/PythonQtScriptingConsole.h>

#include <QApplication>

//#include "MainWindow.h"
//#include "RenderTargetView.h"
//#include "GLPlayerWidget.h"
#include "GLWidget.h"

PYTHONQT_EXPORT void PythonQt_init_QtBindings();

const char* gInitScript = 
"import os\n"
"full_path = '../../Data/Scripts/activate.py'\n"
"code = compile(open(full_path).read(), os.path.split(full_path)[-1] ,'exec')\n"
"exec(code, dict(__file__=full_path))\n"
"import sys\n" 
"sys.argv = ['%1']\n"
"sys.path.insert(0,'.')\n"
"sys.path.append('../../Data/Scripts')\n"
        ;
//"#bootstrap\n"
//"full_path = '../../Data/Scripts/bootstrap.py'\n"
//"code = compile(open(full_path).read(), os.path.split(full_path)[-1] ,'exec')\n"
//"exec(code, globals())\n"
//"globals().pop('full_path')\n";

int main( int argc, char **argv )
{
    QApplication application(argc, argv);
#ifdef Q_WS_WIN
	Py_SetPythonHome("../../Data/Scripts");
#endif
    PythonQt::init(PythonQt::RedirectStdOut);
	
//    PythonQt_init_QtBindings();
    
    PythonQtObjectPtr  __main__context = PythonQt::self()->getMainModule();

//    PythonQt::self()->registerClass(&GLPlayerWidget::staticMetaObject,"PIWWidgets", PythonQtCreateObject<GLPlayerWidgetWarpper>);
//    PythonQt::self()->registerClass(&RenderTargetView::staticMetaObject,"PIWWidgets", PythonQtCreateObject<RenderTargetViewWarpper>);
//    PythonQt::self()->registerClass(&GLWidget::staticMetaObject,"PIWWidgets", PythonQtCreateObject<GLWidgetWarpper>);

//    __main__context.addObject("PIWIO", QSimpleInputContext::instance());
//    SPHExp sphexp ;
//    __main__context.addObject("SPH", &sphexp);

    // create main window
//    MainWindow *main_wnd = new MainWindow(NULL);
//    main_wnd->setupScriptingContext();
//    __main__context.addObject("MainWindow", main_wnd);
//    main_wnd->show();

    GLWidget *player = new GLWidget();
    __main__context.addObject("Player", player);
    player->resize(800, 600);
    player->show();

////     create console widget
//    PythonQtScriptingConsole *console = new PythonQtScriptingConsole(0, __main__context);
//    console->resize(400, 200);
//    __main__context.addObject("console", console);
//    console->appendCommandPrompt();
//    console->show();


    // init custom site-packages
    __main__context.evalScript(QString(gInitScript).arg(argv[0]));

    int ret = application.exec();

//    delete main_wnd;
//    delete player;

    return ret;
}

