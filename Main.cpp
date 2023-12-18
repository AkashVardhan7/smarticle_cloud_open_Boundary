// =============================================================================
// Initialize Smarticles from Command Line
// Input from text file
#include "chrono/core/ChStream.h"
#include "chrono/core/ChRealtimeStep.h"
#include "chrono/utils/ChUtilsInputOutput.h"
#include "chrono/physics/ChSystemNSC.h"
#include "chrono/physics/ChSystemSMC.h"
#include "chrono/physics/ChBodyEasy.h"
#include "chrono/physics/ChLinkMotorRotationAngle.h"
#include "chrono_irrlicht/ChVisualSystemIrrlicht.h"
#include "chrono/collision/ChCollisionSystemChrono.h"
#include "chrono/utils/ChUtilsInputOutput.h"
#include "chrono_thirdparty/filesystem/path.h"
#include "Skeleton.cpp"
#include "chrono/core/ChVector.h"
#include <cmath>
#include <sstream>
using namespace chrono;
using namespace chrono::irrlicht;

using namespace irr;
using namespace irr::core;

// -----------------------------------------------------------------------------
// Callback class for contact reporting
// -----------------------------------------------------------------------------



void AddContainer(ChSystem* sys) {

    ChContactMethod contact_method = sys->GetContactMethod();

    // Contact and visualization materials for container
    auto ground_mat = DefaultContactMaterial(contact_method);
    auto ground_mat_vis = chrono_types::make_shared<ChVisualMaterial>(*ChVisualMaterial::Default());
    ground_mat_vis->SetKdTexture(GetChronoDataFile("textures/concrete.jpg"));


    // Create the floor of the rectangular container, using fixed rigid bodies of 'box' type
    auto floorBody = chrono_types::make_shared<ChBodyEasyBox>(5, 0.02, 5, 1000, ground_mat, collision_type);
    floorBody->SetPos(ChVector<>(0, -0.025, 0));
    floorBody->SetBodyFixed(true);
    floorBody->GetVisualShape(0)->SetMaterial(0, ground_mat_vis);
    floorBody->SetCollide(true);
    floorBody->SetNameString("ground 0");
    sys->Add(floorBody);


}



int main(int argc, char* argv[]) {
    GetLog() << "Copyright (c) 2017 projectchrono.org\nChrono version: " << CHRONO_VERSION << "\n\n";
    SetChronoDataPath(CHRONO_DATA_DIR);
    bool useSMC;

    // Read the single line containing all initial positions and orientations
   // std::ifstream infile("\C:\Users\avardhan7\Documents\PROJECT_CHRONO_8\workspace\smarticle_cloud_open_Boundary_build\Release\commands_N7.txt");
    std::ifstream infile("C:/Users/avardhan7/Documents/PROJECT_CHRONO_8/workspace/smarticle_cloud_open_Boundary_build/Release/commands_N10_V2.txt");

    std::string line;
    std::getline(infile, line);
    std::stringstream ss(line);

    std::vector<ChVector<double>> initialPositions;
    std::vector<double> initialOrientations;

    double x, z, theta;
    double smarticle_y_coord = -0.015 + (0.027 / 2);
    while (ss >> x) {
        if (ss.peek() == ',') ss.ignore();
        ss >> z;
        if (ss.peek() == ',') ss.ignore();
        ss >> theta;
        initialPositions.push_back(ChVector<double>(x, smarticle_y_coord, z));
        initialOrientations.push_back(theta);
        if (ss.peek() == ',') ss.ignore();
    }


    // Printing the positions of the robots to check.
    for (const auto& pos : initialPositions) {
        std::cout << "Robot position: " << pos.x() << ", " << pos.y() << ", " << pos.z() << std::endl;
    }



    
    // parse output folder string, assuming in the format of folder/subfolder
    std::string filenames = argv[3];
    if (filesystem::create_subdirectory(filesystem::path(filenames))) {
        std::cout << "  ...Created nested subdirectories" << std::endl;
    } else {
        std::cout << "  ...Error creating subdirectories" << std::endl;
        return 1;        
    }




    ChSystem *sys;
    double step_size;

    if (useSMC){
        ChSystemSMC *my_sys = new ChSystemSMC();
        sys = my_sys;
        step_size = 1e-4;
    } else {
        ChSystemNSC *my_sys = new ChSystemNSC();
        sys = my_sys;
        //step_size = 1e-3;    
        step_size = 1e-4;
    }

    sys->SetCollisionSystemType(collision_type);


    double time_end = 10.0;

    AddContainer(sys);

    // ChVector<float> gravity(0, 0, 9.81);
    ChVector<float> gravity(0, -9.81, 0);
    sys->Set_G_acc(gravity);
    
    // Create and initialize skeletons
    std::vector<Skeleton> skeletons;
    for (size_t i = 0; i < initialPositions.size(); ++i) {
        Skeleton skel(initialPositions[i], initialOrientations[i], 0, 0, 10 + i);
        skel.SetContactMethod(sys->GetContactMethod());
        skel.SetSkeletonName("skel" + std::to_string(i));
        skel.Initialize();
        skel.AddSkeleton(sys);
        skeletons.push_back(skel);
    }


    // Create the Irrlicht visualization system
    
    
    auto vis = chrono_types::make_shared<ChVisualSystemIrrlicht>();
    vis->AttachSystem(sys);
    vis->SetWindowSize(800, 600);
    vis->SetWindowTitle("Gliders");
    vis->Initialize();
    vis->AddLogo();
    vis->AddSkyBox();
    vis->AddCamera(ChVector<>(0, 0.5, 0));
    vis->AddTypicalLights();
    

    int frame = 0;

    // initialize the csv writer to write the output file
    utils::CSV_writer txt(" ");
    txt.stream().setf(std::ios::scientific | std::ios::showpos);
    txt.stream().precision(16);

    int output_freq = 200;
    // while (vis->Run()) {
    while (true){
        // visualization
        // compute dynamics
        sys->DoStepDynamics(step_size);
        
       
        if (frame % output_freq == 0){

            /*char png_filename[100];
            sprintf(png_filename, "img_%01d.jpg", int(frame / 100));
            vis->WriteImageToFile(png_filename);*/



            // Visualization
            
            vis->BeginScene();
            vis->Render();
            vis->ShowInfoPanel(true);
            vis->EndScene();
           
            txt << sys->GetChTime();
            for (auto& skeleton : skeletons) {
                txt << skeleton.GetPos() << skeleton.GetTheta() << skeleton.GetAlpha1() << skeleton.GetAlpha2();
            }
            txt << std::endl;
            

        }

       

        frame++;

        std::cout << "Current_Time_Is" << std::endl;
        std::cout << sys->GetChTime() << std::endl;

        if (sys->GetChTime()>time_end){

            break;
        }

    }

    std::string txt_output;
    if (useSMC){
        txt_output = "SMC_coordinates.txt";
    }
    else {
        txt_output = "NSC_coordinates.txt";

    }
    txt.write_to_file(filenames + '/' + txt_output);


    return 0;
}
