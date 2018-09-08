#pragma once

#include "windows.h"

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

std::string FormatError(unsigned long error_code_);
std::string FormatHex(std::vector<unsigned char> input_vector_);
