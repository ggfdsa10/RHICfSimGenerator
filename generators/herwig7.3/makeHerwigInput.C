#include <iostream>
#include <fstream>

void makeHerwigInput(int jobIdx = -1, TString runType = "", TString outdataPath = "", TString saveRHICfNeu = "")
{
    TObjArray *tokens;
    if(jobIdx == -1){return;}
    if(runType == ""){return;}
    int tmpRunType = -1;
    if(runType == "TL" || runType == "0"){tmpRunType = 0;}
    if(runType == "TS" || runType == "1"){tmpRunType = 1;}
    if(runType == "TOP" || runType == "2"){tmpRunType = 2;}

    TString inputFileName = Form("run_RHIC-MB_%i.in", jobIdx);

    std::ifstream templateFile("RHIC-MB_template.in");
    std::ofstream inputFile(inputFileName);

    if(templateFile.is_open()){
        if(inputFile.is_open()){
            string lineTmp;
            while(getline(templateFile, lineTmp)){
                TString tempLine = lineTmp;
                
                if(tempLine.Index("HwRHICfHandler:RHICfRunType") != -1){ // for RHICfRunType
                    tokens = tempLine.Tokenize(" ");
                    TString token0 = ((TObjString *) tokens -> At(0)) -> GetString();
                    TString token1 = ((TObjString *) tokens -> At(1)) -> GetString();
                    TString modLine = token0 + " " + token1 + " " + Form("%i", tmpRunType);

                    inputFile << modLine << endl;
                }
                else if(tempLine.Index("HwRHICfHandler:OutputPath") != -1){ 
                    tokens = tempLine.Tokenize(" ");
                    TString token0 = ((TObjString *) tokens -> At(0)) -> GetString();
                    TString token1 = ((TObjString *) tokens -> At(1)) -> GetString();
                    TString modLine = token0 + " " + token1 + " " + outdataPath;

                    inputFile << modLine << endl;
                }
                else if(tempLine.Index("HwRHICfHandler:SaveOption") != -1){ 
                    if(saveRHICfNeu == "n" || saveRHICfNeu == "neutron"){
                        tokens = tempLine.Tokenize(" ");
                        TString token0 = ((TObjString *) tokens -> At(0)) -> GetString();
                        TString token1 = ((TObjString *) tokens -> At(1)) -> GetString();
                        TString modLine = token0 + " " + token1 + " " + saveRHICfNeu;

                        inputFile << modLine << endl;
                    }
                    else{
                        continue;
                    }
                }
                else if(tempLine.Index("saverun") != -1){ 
                    tokens = tempLine.Tokenize(" ");
                    TString token0 = ((TObjString *) tokens -> At(0)) -> GetString();
                    TString token2 = ((TObjString *) tokens -> At(2)) -> GetString();
                    TString modLine = token0 + " " + Form("run_RHIC-MB_%i", jobIdx) + " " + token2;

                    inputFile << modLine << endl;
                }
                else {
                    inputFile << tempLine << endl;
                }
            }
            inputFile.close();
        }
        templateFile.close();
    }
    

}