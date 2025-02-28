#include "RHICfSimManager.hh"

RHICfSimManager::RHICfSimManager() 
: mModel(-1), mRHICfRunType(-1), mFilePath(""), mFileName(""), mCurrEventIdx(0), mSavePi0Neutron(false)
{
}

RHICfSimManager::~RHICfSimManager()
{
    if(mTree){delete mTree;}
    if(mFile){delete mFile;}
    if(mRHICfPoly){delete mRHICfPoly;}
}

void RHICfSimManager::Init()
{
    cout << "RHICfSimManager::Init() -- Initialization... " << endl;

    if(mModel == -1){
        cout<< "RHICfSimManager::Init() -- Not set the Simulation model!!!, model index: " << mModel << endl; 
        return;
    }
    mRndEngine = new TRandom3(0);

    InitWrite();
    
    mParticleArr = new TClonesArray("TParticle");
    mTree -> Branch("particle", &mParticleArr);
    mTree -> Branch("process", &mEventProcess, "process/I");
    mTree -> Branch("sHat", &mEvent_sHat, "sHat/D");
    mTree -> Branch("tHat", &mEvent_tHat, "tHat/D");
    mTree -> Branch("uHat", &mEvent_uHat, "uHat/D");

    InitRHICfGeometry();
}

void RHICfSimManager::SetSimModel(int model)
{
    mModel = model;
}

void RHICfSimManager::SetFilePath(TString path)
{
    mFilePath = path;
}

void RHICfSimManager::SetRHICfRunType(int type)
{
    mRHICfRunType = type;
}

void RHICfSimManager::Clear()
{
    mParticleArr -> Clear("C");
    mEventProcess = -999;
    mEvent_sHat = -999;
    mEvent_tHat = -999;
    mEvent_uHat = -999;
}

void RHICfSimManager::SetEventInfo(int processId, double sHat, double tHat, double uHat)
{
    mEventProcess = processId;
    mEvent_sHat = sHat;
    mEvent_tHat = tHat;
    mEvent_uHat = uHat;
}

void RHICfSimManager::Fill()
{
    mTree -> Fill();
}

void RHICfSimManager::Close()
{
    mFile -> cd();
    mTree -> Write();
    mFile -> Close();

    cout << "RHICfSimManager::Close() -- event generation has done !!!" << endl; 
    cout << "RHICfSimManager::Close() -- Saved file: " << mFileName << endl; 
}

void RHICfSimManager::InitWrite()
{   
    if(mFilePath == ""){mFilePath = gSystem->pwd();}
    TSystemDirectory dir("dir", mFilePath);

    TList *listOfFiles = dir.GetListOfFiles();
    TIter next(listOfFiles);
    TObject *objDir;
    TSystemFile* dataDir; 
    TObjArray *tokens;

    TString filterTypeName = "pi0";
    if(mSavePi0Neutron == true){filterTypeName = "all";}

    int maxVerIdx = 0;
    while((objDir = next())){
        dataDir = dynamic_cast<TSystemFile*>(objDir);
        if(dataDir && !dataDir->IsDirectory()){
            TString dataName = dataDir->GetName();
            if(dataName.Index("RHICfSimGen.root") != -1){
                if(dataName.Index(Form("%s", kModelName[mModel].Data())) != -1){
                    if(dataName.Index(Form("%s", kRHICfRunTypeName[mRHICfRunType].Data())) != -1){
                        if(dataName.Index(Form("%s", filterTypeName.Data())) != -1){
                            tokens = dataName.Tokenize("_");
                            TString token = ((TObjString *) tokens -> At(tokens->GetEntries()-1)) -> GetString();
                            tokens = token.Tokenize(".");
                            token = ((TObjString *) tokens -> At(0)) -> GetString();
                            if(maxVerIdx < token.Atoi()){maxVerIdx = token.Atoi();}
                        }
                    }
                }
            }
        }
    }

    if(mFilePath[mFilePath.Sizeof()-2] != '/'){mFilePath = mFilePath + "/";}
    mFileName = Form("%s%s_%s_%s_%i.RHICfSimGen.root", mFilePath.Data(), kModelName[mModel].Data(), kRHICfRunTypeName[mRHICfRunType].Data(), filterTypeName.Data(), maxVerIdx+1);

    mFile = new TFile(mFileName.Data(), "recreate");
    mTree = new TTree("Event", "Event");
}

void RHICfSimManager::InitRHICfGeometry()
{
    double mRHICfTowerBoundary[2][4][2]; // [TS, TL][bound square][x, y]
    double mRHICfTowerCenterPos[2]; // [TS, TL] y pos

    double tsDetSize = 20.; // [mm]
    double tlDetSize = 40.; // [mm]
    double detBoundCut = 0.; // [mm]
    double distTStoTL = 47.4; // [mm]
    double detBeamCenter = 0.; // [mm]

    if(mRHICfRunType < kTL || mRHICfRunType > kTOP){
        cout << "RHICfSimManager::InitRHICfGeometry() warning!!! RHICf run type is not setted!!!" << endl;
        return;
    }

    if(mRHICfRunType == kTL){detBeamCenter = -47.4;} // TL
    if(mRHICfRunType == kTS){detBeamCenter = 0.;} // TS
    if(mRHICfRunType == kTOP){detBeamCenter = 21.6;} // TOP

    mRHICfTowerBoundary[0][0][0] = sqrt(2)*((tsDetSize - detBoundCut*2.)/2.); 
    mRHICfTowerBoundary[0][0][1] = 0.;
    mRHICfTowerBoundary[0][1][0] = 0.; 
    mRHICfTowerBoundary[0][1][1] = sqrt(2)*((tsDetSize - detBoundCut*2.)/2.); 
    mRHICfTowerBoundary[0][2][0] = -1.*sqrt(2)*((tsDetSize - detBoundCut*2.)/2.); 
    mRHICfTowerBoundary[0][2][1] = 0.; 
    mRHICfTowerBoundary[0][3][0] = 0.; 
    mRHICfTowerBoundary[0][3][1] = -1.*sqrt(2)*((tsDetSize - detBoundCut*2.)/2.); 

    mRHICfTowerBoundary[1][0][0] = sqrt(2)*((tlDetSize - detBoundCut*2.)/2.);
    mRHICfTowerBoundary[1][0][1] = 0.;
    mRHICfTowerBoundary[1][1][0] = 0.;
    mRHICfTowerBoundary[1][1][1] = sqrt(2)*((tlDetSize - detBoundCut*2.)/2.);
    mRHICfTowerBoundary[1][2][0] = -1.*sqrt(2)*((tlDetSize - detBoundCut*2.)/2.);
    mRHICfTowerBoundary[1][2][1] = 0.;
    mRHICfTowerBoundary[1][3][0] = 0.;
    mRHICfTowerBoundary[1][3][1] = -1.*sqrt(2)*((tlDetSize - detBoundCut*2.)/2.);
    
    mRHICfTowerCenterPos[0] = detBeamCenter;
    mRHICfTowerCenterPos[1] = distTStoTL + detBeamCenter;

    mRHICfPoly = new TH2Poly();
    mRHICfPoly -> SetName("RHICfPoly");
    mRHICfPoly -> SetStats(0);

    double x[4];
    double y[4];

    for(int t=0; t<2; t++){
        for(int i=0; i<4; i++){
        double xPos = mRHICfTowerBoundary[t][i][0];
        double yPos = mRHICfTowerCenterPos[t] + mRHICfTowerBoundary[t][i][1];
        x[i] = xPos;
        y[i] = yPos;
        }
        mRHICfPoly -> AddBin(4, x, y);
    }
}

Int_t RHICfSimManager::GetRHICfGeoHit(double posX, double posY, double posZ, double px, double py, double pz, double e)
{
  if(e < 20.){return -1;} // energy cut

  double momMag = sqrt(px*px + py*py + pz*pz);
  double unitVecX = px/momMag;
  double unitVecY = py/momMag;
  double unitVecZ = pz/momMag;

  if(unitVecZ < 0){return -1;} // opposite side cut

  double mRHICfDetZ = 17800.; // [mm]
  double z = mRHICfDetZ - posZ;
  if(z < 0.){return -1;} // create z-position cut

  double x = z * (unitVecX/unitVecZ) + posX;
  double y = z * (unitVecY/unitVecZ) + posY;

  int type = mRHICfPoly -> FindBin(x, y);
  if(type < 1 || type > 2){return -1;} // RHICf geometrical hit cut

  return type;
}

void RHICfSimManager::GetVertexRandom(double& vx, double& vy, double& vz)
{
    vx = mRndEngine -> Gaus(0., vertexXSigma);
    vy = mRndEngine -> Gaus(0., vertexYSigma);
    vz = mRndEngine -> Gaus(0., vertexZSigma);
}