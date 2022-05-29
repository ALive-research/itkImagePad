// ITK includes
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkConstantPadImageFilter.h>

// TCLAP includes
#include <tclap/ValueArg.h>
#include <tclap/ArgException.h>
#include <tclap/CmdLine.h>
#include <tclap/SwitchArg.h>

// STD includes
#include <cstdlib>

//NOTE: Here we assume the input and output data type is unsigned short!

int main (int argc, char **argv)
{

  // =========================================================================
  // Command-line variables
  // =========================================================================
  std::string inputImage;
  std::string outputImage;
  unsigned short pad;
  unsigned short value;


  // =========================================================================
  // Parse arguments
  // =========================================================================
  try {

    TCLAP::CmdLine cmd("itkBoundingBox");

    TCLAP::ValueArg<std::string> input("i", "input", "Image to compute the output", true, "None", "string");
    TCLAP::ValueArg<std::string> output("o", "output", "Cropped imge", true, "None", "string");
    TCLAP::ValueArg<unsigned short> inputPad("p", "padding", "Size of pad", true, 1, "unsinged short");
    TCLAP::ValueArg<unsigned short> inputValue("v", "value", "Value of pad", true, 0, "unsinged short");

    cmd.add(input);
    cmd.add(output);
    cmd.add(inputPad);
    cmd.add(inputValue);

    cmd.parse(argc,argv);

    inputImage = input.getValue();
    outputImage = output.getValue();
    pad = inputPad.getValue();
    value = inputValue.getValue();

  } catch (TCLAP::ArgException &e) {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    return EXIT_FAILURE;
  }

  // =========================================================================
  // ITK definitions
  // =========================================================================
  using ImageType = itk::Image<unsigned short, 3>;
  using ImageReaderType = itk::ImageFileReader<ImageType>;
  using ImageWriterType = itk::ImageFileWriter<ImageType>;
  using PadFilterType= itk::ConstantPadImageFilter<ImageType, ImageType>;

  // =========================================================================
  // Image loading and checking
  // =========================================================================
  auto inputReader = ImageReaderType::New();
  inputReader->SetFileName(inputImage);
  inputReader->Update();

  // =========================================================================
  // Add the padding
  // =========================================================================
  const ImageType::SizeValueType upperBound[3] = {pad, pad, pad};
  const ImageType::SizeValueType lowerBound[3] = {pad, pad, pad};
  auto paddingFilter = PadFilterType::New();
  paddingFilter->SetInput(inputReader->GetOutput());
  paddingFilter->SetPadUpperBound(upperBound);
  paddingFilter->SetPadLowerBound(lowerBound);
  paddingFilter->SetConstant(value);
  paddingFilter->Update();

  // =========================================================================
  // Write the sub-volume
  // =========================================================================
  auto outputWriter = ImageWriterType::New();
  outputWriter->SetFileName(outputImage);
  outputWriter->SetInput(paddingFilter->GetOutput());
  outputWriter->Write();

  return EXIT_SUCCESS;
}
