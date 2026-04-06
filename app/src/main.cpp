#include <QApplication>
#include <QFont>
#include <QFontDatabase>
#include "MainWindow.hpp"
#include "AppController.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Setting application metadata
    app.setApplicationName("PocketTrack");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Marks Oriented Programming");
    
    // Setting default font
    QFont defaultFont("Segoe UI", 10);
    app.setFont(defaultFont);
    
    // Creating controller and main window
    AppController controller;
    
    MainWindow mainWindow;
    mainWindow.setController(&controller);
    mainWindow.show();
    
    // Loading songs from CSV 
    QString csvPath = "data/songs.csv";
    if (!controller.loadSongsFromCSV(csvPath)) {
        csvPath = "D:/oop_proj_2_CL/Pocket_Tracker/data/songs.csv";
        controller.loadSongsFromCSV(csvPath);
    }
    
    // Updating the UI with loaded songs
    mainWindow.showLibrary();
    
    return app.exec();
}