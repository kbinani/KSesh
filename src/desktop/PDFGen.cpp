#include "pdfgen.c"

#include "PDFGen.hpp"

static int pdf_save_object_stream(struct pdf_doc *pdf, std::string &out, int index) {
  struct pdf_object *object = pdf_get_object(pdf, index);
  if (!object)
    return -ENOENT;

  if (object->type == OBJ_none)
    return -ENOENT;

  object->offset = out.size(); // ftell(fp)

  //fprintf(fp, "%d 0 obj\r\n", index);
  out += std::format("{} 0 obj\r\n", index);

  switch (object->type) {
  case OBJ_stream:
  case OBJ_image: {
    //fwrite(dstr_data(&object->stream.stream),
    //       dstr_len(&object->stream.stream), 1, fp);
    std::copy_n(dstr_data(&object->stream.stream), dstr_len(&object->stream.stream), std::back_inserter(out));
    break;
  }
  case OBJ_info: {
    struct pdf_info *info = object->info;

    //fprintf(fp, "<<\r\n");
    out += "<<\r\n";
    if (info->creator[0]) {
      //fprintf(fp, "  /Creator (%s)\r\n", info->creator);
      out += std::format("  /Creator ({})\r\n", info->creator);
    }
    if (info->producer[0]) {
      //fprintf(fp, "  /Producer (%s)\r\n", info->producer);
      out += std::format("  /Producer ({})\r\n", info->producer);
    }
    if (info->title[0]) {
      //fprintf(fp, "  /Title (%s)\r\n", info->title);
      out += std::format("  /Title ({})\r\n", info->title);
    }
    if (info->author[0]) {
      //fprintf(fp, "  /Author (%s)\r\n", info->author);
      out += std::format("  /Author ({})\r\n", info->author);
    }
    if (info->subject[0]) {
      //fprintf(fp, "  /Subject (%s)\r\n", info->subject);
      out += std::format("  /Subject ({})\r\n", info->subject);
    }
    if (info->date[0]) {
      //fprintf(fp, "  /CreationDate (D:%s)\r\n", info->date);
      out += std::format("  /CreationDate (D:{})\r\n", info->date);
    }
    //fprintf(fp, ">>\r\n");
    out += ">>\r\n";
    break;
  }

  case OBJ_page: {
    struct pdf_object *pages = pdf_find_first_object(pdf, OBJ_pages);
    bool printed_xobjects = false;

    //fprintf(fp,
    //        "<<\r\n"
    //        "  /Type /Page\r\n"
    //        "  /Parent %d 0 R\r\n",
    //        pages->index);
    out += std::format("<<\r\n"
             "  /Type /Page\r\n"
             "  /Parent {:d} 0 R\r\n",             pages->index);
    //fprintf(fp, "  /MediaBox [0 0 %f %f]\r\n", object->page.width,            object->page.height);
    out += std::format("  /MediaBox [0 0 {:f} {:f}]\r\n", object->page.width, object->page.height);
    out += "  /Resources <<\r\n";
    out += "    /Font <<\r\n";
    for (struct pdf_object *font = pdf_find_first_object(pdf, OBJ_font);
         font; font = font->next)
      out += std::format("      /F{:d} {:d} 0 R\r\n", font->font.index,   font->index);
    out += "    >>\r\n";
    // We trim transparency to just 4-bits
    out += "    /ExtGState <<\r\n";
    for (int i = 0; i < 16; i++) {
      out += std::format("      /GS{:d} <</ca {:f}>>\r\n", i,              (float)(15 - i) / 15);
    }
    out += "    >>\r\n";

    for (struct pdf_object *image = pdf_find_first_object(pdf, OBJ_image);
         image; image = image->next) {
      if (image->stream.page == object) {
        if (!printed_xobjects) {
          out += "    /XObject <<";
          printed_xobjects = true;
        }
        out += std::format("      /Image{:d} {:d} 0 R ", image->index,                image->index);
      }
    }
    if (printed_xobjects) {
      out += "    >>\r\n";
    }
    out += "  >>\r\n";

    out += "  /Contents [\r\n";
    for (int i = 0; i < flexarray_size(&object->page.children); i++) {
      struct pdf_object *child =
          (struct pdf_object *)flexarray_get(&object->page.children, i);
      out += std::format("{:d} 0 R\r\n", child->index);
    }
    out += "]\r\n";

    if (flexarray_size(&object->page.annotations)) {
      out += "  /Annots [\r\n";
      for (int i = 0; i < flexarray_size(&object->page.annotations);
           i++) {
        struct pdf_object *child = (struct pdf_object *)flexarray_get(
            &object->page.annotations, i);
        out += std::format("{:d} 0 R\r\n", child->index);
      }
      out += "]\r\n";
    }

    out += ">>\r\n";
    break;
  }

  case OBJ_bookmark: {
    struct pdf_object *parent, *other;

    parent = object->bookmark.parent;
    if (!parent)
      parent = pdf_find_first_object(pdf, OBJ_outline);
    if (!object->bookmark.page)
      break;
    out += std::format(
            "<<\r\n"
            "  /Dest [{:d} 0 R /XYZ 0 {:f} null]\r\n"
            "  /Parent {:d} 0 R\r\n"
            "  /Title ({:s})\r\n",
            object->bookmark.page->index, pdf->height, parent->index,
            object->bookmark.name);
    int nchildren = flexarray_size(&object->bookmark.children);
    if (nchildren > 0) {
      struct pdf_object *f, *l;
      f = (struct pdf_object *)flexarray_get(&object->bookmark.children,
                                             0);
      l = (struct pdf_object *)flexarray_get(&object->bookmark.children,
                                             nchildren - 1);
      out += std::format("  /First {:d} 0 R\r\n", f->index);
      out += std::format("  /Last {:d} 0 R\r\n", l->index);
      out += std::format("  /Count {:d}\r\n", pdf_get_bookmark_count(object));
    }
    // Find the previous bookmark with the same parent
    for (other = object->prev;
         other && other->bookmark.parent != object->bookmark.parent;
         other = other->prev)
      ;
    if (other) {
      out += std::format("  /Prev {:d} 0 R\r\n", other->index);
    }
    // Find the next bookmark with the same parent
    for (other = object->next;
         other && other->bookmark.parent != object->bookmark.parent;
         other = other->next)
      ;
    if (other) {
      out += std::format("  /Next {:d} 0 R\r\n", other->index);
    }
    out += ">>\r\n";
    break;
  }

  case OBJ_outline: {
    struct pdf_object *first, *last, *cur;
    first = pdf_find_first_object(pdf, OBJ_bookmark);
    last = pdf_find_last_object(pdf, OBJ_bookmark);

    if (first && last) {
      int count = 0;
      cur = first;
      while (cur) {
        if (!cur->bookmark.parent)
          count += pdf_get_bookmark_count(cur) + 1;
        cur = cur->next;
      }

      /* Bookmark outline */
      out += std::format(
              "<<\r\n"
              "  /Count {:d}\r\n"
              "  /Type /Outlines\r\n"
              "  /First {:d} 0 R\r\n"
              "  /Last {:d} 0 R\r\n"
              ">>\r\n",
              count, first->index, last->index);
    }
    break;
  }

  case OBJ_font:
    out += std::format(
            "<<\r\n"
            "  /Type /Font\r\n"
            "  /Subtype /Type1\r\n"
            "  /BaseFont /{:s}\r\n"
            "  /Encoding /WinAnsiEncoding\r\n"
            ">>\r\n",
            object->font.name);
    break;

  case OBJ_pages: {
    int npages = 0;

    out +=  "<<\r\n"
                "  /Type /Pages\r\n"
                "  /Kids [ ";
    for (struct pdf_object *page = pdf_find_first_object(pdf, OBJ_page);
         page; page = page->next) {
      npages++;
      out += std::format("{:d} 0 R ", page->index);
    }
    out += "]\r\n";
    out += std::format("  /Count {:d}\r\n", npages);
    out += ">>\r\n";
    break;
  }

  case OBJ_catalog: {
    struct pdf_object *outline = pdf_find_first_object(pdf, OBJ_outline);
    struct pdf_object *pages = pdf_find_first_object(pdf, OBJ_pages);

    out += "<<\r\n"
                "  /Type /Catalog\r\n";
    if (outline) {
      out += std::format(
              "  /Outlines {:d} 0 R\r\n"
              "  /PageMode /UseOutlines\r\n",
              outline->index);
    }
    out += std::format(            "  /Pages {:d} 0 R\r\n"
            ">>\r\n",
            pages->index);
    break;
  }

  case OBJ_link: {
    out += std::format(
            "<<\r\n"
            "  /Type /Annot\r\n"
            "  /Subtype /Link\r\n"
            "  /Rect [{:f} {:f} {:f} {:f}]\r\n"
            "  /Dest [{} 0 R /XYZ {:f} {:f} null]\r\n"
            "  /Border [0 0 0]\r\n"
            ">>\r\n",
            object->link.llx, object->link.lly, object->link.urx,
            object->link.ury, object->link.target_page->index,
            object->link.target_x, object->link.target_y);
    break;
  }

  default:
    return pdf_set_err(pdf, -EINVAL, "Invalid PDF object type %d",
                       object->type);
  }

  out += "endobj\r\n";

  return 0;
}

int pdf_save_stream(struct pdf_doc *pdf, std::string &out) {
  struct pdf_object *obj;
  int xref_offset;
  int xref_count = 0;
  uint64_t id1, id2;
  time_t now = time(NULL);
  //char saved_locale[32];

  //force_locale(saved_locale, sizeof(saved_locale));

  out += "%%PDF-1.3\r\n";
  /* Hibit bytes */
  out += std::format("{:c}{:c}{:c}{:c}{:c}\r\n", (char)0x25, (char)0xc7, (char)0xec, (char)0x8f, (char)0xa2);

  /* Dump all the objects & get their file offsets */
  for (int i = 0; i < flexarray_size(&pdf->objects); i++)
    if (pdf_save_object_stream(pdf, out, i) >= 0)
      xref_count++;

  /* xref */
  xref_offset = out.size(); // ftell(fp);
  out += "xref\r\n";
  out += std::format("0 {:d}\r\n", xref_count + 1);
  out += "0000000000 65535 f\r\n";
  for (int i = 0; i < flexarray_size(&pdf->objects); i++) {
    obj = pdf_get_object(pdf, i);
    if (obj->type != OBJ_none) {
      out += std::format("{:010d} 00000 n\r\n", obj->offset); // %10.10d
    }
  }

  out += std::format(
      "trailer\r\n"
      "<<\r\n"
      "/Size {:d}\r\n",
      xref_count + 1);
  obj = pdf_find_first_object(pdf, OBJ_catalog);
  out += std::format("/Root {:d} 0 R\r\n", obj->index);
  obj = pdf_find_first_object(pdf, OBJ_info);
  out += std::format("/Info {:d} 0 R\r\n", obj->index);
  /* Generate document unique IDs */
  id1 = hash(5381, obj->info, sizeof(struct pdf_info));
  id1 = hash(id1, &xref_count, sizeof(xref_count));
  id2 = hash(5381, &now, sizeof(now));
  out += std::format("/ID [<{:016d}> <{:016d}>]\r\n", id1, id2);
  out += ">>\r\n"
         "startxref\r\n";
  out += std::format("{:d}\r\n", xref_offset);
  out += "%%%%EOF\r\n";

  //restore_locale(saved_locale);

  return 0;
}

