#include <iostream>
#include "components/resource/resourcesystem.hpp"
#include "components/resource/scenemanager.hpp"
#include "components/vfs/manager.hpp"
#include "components/vfs/bsaarchive.hpp"
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

char nonstrict_normalize_char(char ch)
{
    return ch == '\\' ? '/' : Misc::StringUtils::toLower(ch);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        std::cout << "it should be " << argv[0] << " bsa modelName" << std::endl;
        return 0;
    }

    std::string filename(argv[1]);
    std::string modelName(argv[2]);
    bool strict = false;

    VFS::BsaArchive* bsaArchive = new VFS::BsaArchive(filename);

    VFS::Manager manager(strict);
    manager.addArchive(bsaArchive);
    manager.buildIndex();
    
    Resource::ResourceSystem resourceSystem(&manager);

    auto sceneManager = resourceSystem.getSceneManager();

    osg::ref_ptr<osg::Node> model = sceneManager->getInstance(modelName);

    osgViewer::Viewer viewer;
    viewer.addEventHandler(new osgViewer::StatsHandler);
    viewer.setSceneData(model);
    
    return viewer.run();
}
