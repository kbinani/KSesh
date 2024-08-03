#pragma once
#include "pdfgen.h"

#include <string>
#include <format>

int pdf_save_stream(struct pdf_doc *pdf, std::string &out);
