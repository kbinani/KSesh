#pragma once
#include "pdfgen.h"

#include <format>
#include <string>

int pdf_save_stream(struct pdf_doc *pdf, std::string &out);
