#ifndef __analysis__h__
#define __analysis__h__

#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <exception>

#include "HepMC3/GenEvent.h"
#include "HepMC3/ReaderAscii.h"
#include "HepMC3/WriterAscii.h"
#include "HepMC3/Print.h"

#include <iostream>

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zlib.hpp>


class DataManager
{
private:
  std::vector<std::string> filelist;
  std::vector<std::string>::iterator current_file;

  boost::iostreams::filtering_istream in;
  HepMC3::ReaderAscii* ascii_in;
  int count;


  bool GetNextFile()
  {
    if(++current_file == filelist.end())
      return 0;
    return ReadFile();
  }

  bool ReadFile()
  {
    std::string filename = *current_file;
    bool use_compression = false;
    if(filename.find("gz") == filename.length()-2) use_compression = true;
    std::cout << "DataMangager::Opening file " << filename << " with" << (use_compression?" ":"out ") << "compression. " << std::endl;

    in.reset();
    if(use_compression)
      in.push (boost::iostreams::gzip_decompressor ());
    in.push (boost::iostreams::file_descriptor_source (filename));
    if(ascii_in) delete ascii_in;
    ascii_in = new HepMC3::ReaderAscii(in);
    return true;
  }

public:
  HepMC3::GenEvent evt;
  DataManager() : ascii_in(0), count(0)
  {

  }
  ~DataManager()
  {
    if(ascii_in) delete ascii_in;
    std::cout << "DataManger::Closing after reading " << count << " events." << std::endl;
  }

  void SetFiles(const std::vector<std::string>& files)
  {
    filelist = files; //copy
    //open first file
    current_file = filelist.begin();
    ReadFile();
  }


  bool GetNextEvent()
  {
    try
      {
        ascii_in->read_event(evt);
      }
    catch (std::exception& e)
      {
        std::cout << "Event could not be read: " << e.what() << std::endl;
      }
    if (ascii_in->failed())
      {
	if (!GetNextFile()) return false;
	else // new file selected
	  {
	    try
              {
		ascii_in->read_event(evt);
              }
            catch(std::exception& e)
              {
                std::cout << "Event could not be read again: " << e.what() << std::endl;
              }
	    if (ascii_in->failed())  return false;
	  }
      }

    if(count % 1000 == 0)
      std::cout << "Reading event: " << count << std::endl;
    count++;
    return true;
  }

};


#endif
