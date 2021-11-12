#include "MyRoot.hpp"

/* Constructor, destructor. */ 
MyRoot::MyRoot(std::string mod,
                           std::map<std::string, std::string> params,
                           std::vector<std::string> inputs,
                           std::vector<std::string> outputs)
{
  _mod = mod;
  _params = params;
  _inputs = inputs;
  _outputs = outputs;
  _log = "";
}
MyRoot::~MyRoot()
{
}
//------------------------------------------------------------------------------

/* Get functions. */ 
std::string MyRoot::getLog() 
{
  std::string result = _logSymbol + _rowDelimiter;
  result += _log;
  std::string sha = this->findParam("SHA", &_params);
  result += "Version(GitHub SHA): " + sha + _rowDelimiter;
 
  return result;
}
//------------------------------------------------------------------------------

/* Loop Functions. */ 
bool MyRoot::TGraphErrorsTable() 
{
  // Configure Params
  
  std::vector<std::string> parserArrayDelimiters;
  parserArrayDelimiters.push_back(this->findParam("sys&DqParser&AD&1", &_params));
  parserArrayDelimiters.push_back(this->findParam("sys&DqParser&AD&2", &_params));
  DqParser parser; parser.setArrayDelimiter(parserArrayDelimiters);
  
  std::string dataFormat;
  bool isDataFormat = this->findParam("dataFormat", &dataFormat, &_params);
  if(!isDataFormat)
    return false;
  
  std::string sColumRow; 
  bool isColumRow = this->findParam("colum|row", &sColumRow, &_params);
  if(!isColumRow)
    return false;
  int iColumRow = atoi(sColumRow.c_str());  
  bool columRow = (bool)(iColumRow);

  std::string columDelimiter = this->findParam("columDelimiter", &_params); 
  std::string rowDelimiter = this->findParam("rowDelimiter", &_params); 
  
  std::string name = this->findParam("name", &_params); 
  std::string resultName = ""; 
  std::string title = this->findParam("title", &_params); 
  std::string xTitle = this->findParam("xTitle", &_params); 
  std::string yTitle = this->findParam("yTitle", &_params); 
  
  std::string sIdX = this->findParam("idX", &_params);
  int iX = atoi(sIdX.c_str());
  std::string sIdEX = this->findParam("idEX", &_params); 
  int iEX = atoi(sIdEX.c_str());
  std::string sIdY = this->findParam("idY", &_params); 
  int iY = atoi(sIdY.c_str());
  std::string sIdEY = this->findParam("idEY", &_params); 
  int iEY = atoi(sIdEY.c_str());
  
  std::string sData = this->findParam("data", &_params);
  std::vector<std::string> data = parser.parsArray(sData);
  
  std::string sSkipEX = this->findParam("skipEX", &_params);
  std::vector<std::string> skipEX = parser.parsArray(sSkipEX);
  
  std::string sSkipEY = this->findParam("skipEY", &_params);
  std::vector<std::string> skipEY = parser.parsArray(sSkipEY);
  //----------------------------------------------------------------------------
  
  std::vector<std::string> sX;
  std::vector<std::string> sEX;
  std::vector<std::string> sY;
  std::vector<std::string> sEY;
  std::vector<double> x; 
  std::vector<double> ex; 
  std::vector<double> y; 
  std::vector<double> ey;
  std::vector<int> sizes;

  std::string dirRoot;
  if(_outputs.size() != 1)
    return false;

  DqRoot dataRoot;
  dataRoot.setOutputFile(_outputs.at(0), "default");

  DqProcessBar processBar;
  std::cout << "Processing: ";
  for(unsigned int i = 0; i < _inputs.size(); i++) {
    std::cout << std::endl << "File[" << i << "]: " << parser.fileName(_inputs.at(i)) << std::endl;
    if(_inputs.size() > 1) {
      std::string tmp = parser.fileName(_inputs.at(i));
      parser.deleteFormat(&tmp);
      resultName = name + "_" + tmp;
    }

      if(dataFormat == "CSV") {
        // create stream and open file
        DqCSV inCSV(_inputs.at(i), columDelimiter, _rowDelimiter);
        bool isOpenInCSV = inCSV.open(std::fstream::in);
        if(!isOpenInCSV) {
          _log += " WARNING! file: " + _inputs.at(i) + 
                  " was skipped! Can't open!" + _rowDelimiter; 
          continue;
      }
      // read full colum|row -- need add cell filling type (big colum|row)
      sX.clear(); sEX.clear(); sY.clear(); sEY.clear();
      if(columRow) {
        inCSV.readColumn(iX, &sX);
        inCSV.readColumn(iEX, &sEX);
        inCSV.readColumn(iY, &sY);
        inCSV.readColumn(iEY, &sEY);
      }
      else {
        inCSV.readRow(iX, &sX);
        inCSV.readRow(iEX, &sEX);
        inCSV.readRow(iY, &sY);
        inCSV.readRow(iEY, &sEY);
      }
      // close file
      if(isOpenInCSV) {
        bool isCloseInCSV = false; 
        isCloseInCSV = inCSV.close();  
        if(!isCloseInCSV) return false;
      }
    }
    else {
      _log += " WARNING! file: " + _inputs.at(i) + 
              " was skipped! Don't know dataFormat!" + _rowDelimiter; 
      continue;
    }
    sizes.clear();
    sizes.push_back(sX.size()); sizes.push_back(sEX.size()); 
    sizes.push_back(sY.size()); sizes.push_back(sEX.size()); 
    if(!(count(sizes.begin(), sizes.end(), sizes[0]) == (int)sizes.size())) {
      _log += " WARNING! file: " + _inputs.at(i) + 
              " was skipped! Size of colum or row not equal!" + _rowDelimiter;
      continue;
    }
    x.clear(); ex.clear(); y.clear(); ey.clear();
    // no good part of code beg
    unsigned int index = 0;
    bool flag = false;
    processBar.setEndIndex(sX.size()-1);
    processBar.setCurrentStep(0);
    for(unsigned int j = 0; j < sX.size(); j++) {
      flag = false;
      for(unsigned int jj = 0; jj < data.size(); jj++) {
        index = atoi(data.at(jj).c_str());
        if(index == j) {
          flag = true;
          break;
        }
      }
      if(flag) {
        x.push_back(atof(sX.at(j).c_str()));
        y.push_back(atof(sY.at(j).c_str()));
        flag = false;
        ex.push_back(atof(sEX.at(j).c_str()));
        for(unsigned int jj = 0; jj < skipEX.size(); jj++) {
          index = atoi(skipEX.at(jj).c_str());
          if(index == j) {
            flag = true;
            ex.pop_back();
            ex.push_back(0);
            break;
          }
        }
        flag = false;
        ey.push_back(atof(sEY.at(j).c_str()));
        for(unsigned int jj = 0; jj < skipEY.size(); jj++) {
          index = atoi(skipEY.at(jj).c_str());
          if(index == j) {
            flag = true;
            ey.pop_back();
            ey.push_back(0);
            break;
          }
        }
      }
    processBar.print(j, 0.25); 
    }
    // no good part of code end
    TCanvas *c1 = new TCanvas(Form("c_%s", resultName.c_str()), Form("c_%s", title.c_str()));
    c1->SetCanvasSize(700, 600);
    c1->SetWindowSize(700, 600);
    c1->SetGrid();
    TGraphErrors *grE = new TGraphErrors(x.size(), &x[0], &y[0], &ex[0], &ey[0]); 
    grE->SetMarkerStyle(20);
    grE->SetTitle(title.c_str());
    grE->SetName(Form("gr_%s", resultName.c_str()));
    grE->GetXaxis()->SetTitle(xTitle.c_str());
    grE->GetXaxis()->SetTitleOffset(0.7);
    grE->GetXaxis()->SetTitleSize(0.06);
    grE->GetYaxis()->SetTitle(yTitle.c_str());
    grE->GetYaxis()->SetTitleOffset(0.7);
    grE->GetYaxis()->SetTitleSize(0.06);
    grE->Draw("APL");
      
    TString runDir(dirRoot.c_str());
    const char* runDirc = runDir.Data();
    dataRoot.writeObject(runDirc, grE);
    dataRoot.writeObject(runDirc, c1);

    grE->Delete();
    c1->Close();
  }
  std::cout << std::endl;
  std::cout << "End of Processing!" << std::endl;
  std::cout << "#---" << std::endl;
  return true;
}
//------------------------------------------------------------------------------

bool MyRoot::run() 
{
  bool isRun = false;
  if(_mod == "TGraphErrorsTable")
    isRun = TGraphErrorsTable();
   return isRun;
}
//------------------------------------------------------------------------------
