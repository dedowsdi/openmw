#include <iostream>
#include "components/resource/resourcesystem.hpp"
#include "components/resource/scenemanager.hpp"
#include "components/vfs/manager.hpp"
#include "components/vfs/bsaarchive.hpp"
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <boost/program_options.hpp>

char nonstrict_normalize_char(char ch)
{
    return ch == '\\' ? '/' : Misc::StringUtils::toLower(ch);
}

struct Argument{
    std::string file;
    std::string name;
};

int main(int argc, char *argv[])
{
    Argument arg;
    namespace bpo = boost::program_options;
    bpo::options_description desc;
    desc.add_options()
        ("file", bpo::value<std::string>(&arg.file), "data file, otherwise use $BSA_DATA")
        ("name", bpo::value<std::string>(&arg.name), "record name")
        ;

    bpo::positional_options_description pod;
    pod.add("file", 1);

    bpo::variables_map vm;

    bpo::store(bpo::command_line_parser(argc, argv).options(desc).positional(pod).run(), vm);
    bpo::notify(vm);

    bool strict = false;

    VFS::BsaArchive* bsaArchive = new VFS::BsaArchive(arg.file);

    VFS::Manager manager(strict);
    manager.addArchive(bsaArchive);
    manager.buildIndex();

    Resource::ResourceSystem resourceSystem(&manager);

    auto sceneManager = resourceSystem.getSceneManager();

    osg::ref_ptr<osg::Node> model = sceneManager->getInstance(arg.name);

    osgViewer::Viewer viewer;
    viewer.addEventHandler(new osgViewer::StatsHandler);
    viewer.setSceneData(model);

    return viewer.run();
}
