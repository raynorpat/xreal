/*
Copyright (C) 1999-2006 Id Software, Inc. and contributors.
For a list of contributors, see the accompanying CONTRIBUTORS file.

This file is part of GtkRadiant.

GtkRadiant is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

GtkRadiant is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GtkRadiant; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "plugin.h"

#include "debugging/debugging.h"

#include "ifilesystem.h"
#include "iimage.h"

#include "imagelib.h"

// ====== JPEG loader functionality ======

extern "C" {
#define JPEG_INTERNALS
#include "../../../jpeg-6/jpeglib.h"


/*
 * Error exit handler: must not return to caller.
 *
 * Applications may override this if they want to get control back after
 * an error.  Typically one would longjmp somewhere instead of exiting.
 * The setjmp buffer can be made a private field within an expanded error
 * handler object.  Note that the info needed to generate an error message
 * is stored in the error object, so you can generate the message now or
 * later, at your convenience.
 * You should make sure that the JPEG object is cleaned up (with jpeg_abort
 * or jpeg_destroy) at some point.
 */

void jpeg_error_exit(j_common_ptr cinfo)
{
	char            buffer[JMSG_LENGTH_MAX];

	/* Create the message */
	(*cinfo->err->format_message) (cinfo, buffer);

	/* Let the memory manager delete any temp files before we die */
	jpeg_destroy(cinfo);

	globalErrorStream() << "WARNING: JPEG library error: " << buffer << "\n";
}


/*
 * Actual output of an error or trace message.
 * Applications may override this method to send JPEG messages somewhere
 * other than stderr.
 */

void jpeg_output_message(j_common_ptr cinfo)
{
	char            buffer[JMSG_LENGTH_MAX];

	/* Create the message */
	(*cinfo->err->format_message) (cinfo, buffer);

	/* Send it to stderr, adding a newline */
	globalErrorStream() << "WARNING: JPEG library error: " << buffer << "\n";
}

} // extern "C"


/*
=============
LoadJPGBuffer
=============
*/
static Image* LoadJPGBuffer(byte * fbuffer)
{
	/* This struct contains the JPEG decompression parameters and pointers to
	* working space (which is allocated as needed by the JPEG library).
	*/
	struct jpeg_decompress_struct cinfo = {0};

	/* We use our private extension JPEG error handler.
	* Note that this struct must live as long as the main JPEG parameter
	* struct, to avoid dangling-pointer problems.
	*/
	/* This struct represents a JPEG error handler.  It is declared separately
	* because applications often want to supply a specialized error handler
	* (see the second half of this file for an example).  But here we just
	* take the easy way out and use the standard error handler, which will
	* print a message on stderr and call exit() if compression fails.
	* Note that this struct must live as long as the main JPEG parameter
	* struct, to avoid dangling-pointer problems.
	*/
	struct jpeg_error_mgr jerr;

	/* More stuff */
	JSAMPARRAY      buffer;		/* Output row buffer */
	unsigned		row_stride;	/* physical row width in output buffer */
	unsigned		pixelcount;
	unsigned char  *out, *out_converted;
	byte           *bbuf;

	/* In this example we want to open the input file before doing anything else,
	* so that the setjmp() error recovery below can assume the file is open.
	* VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	* requires it in order to read binary files.
	*/

	if(!fbuffer)
	{
		return 0;
	}

	/* Step 1: allocate and initialize JPEG decompression object */

	/* We have to set up the error handler first, in case the initialization
	* step fails.  (Unlikely, but it could happen if you are out of memory.)
	* This routine fills in the contents of struct jerr, and returns jerr's
	* address which we place into the link field in cinfo.
	*/
	cinfo.err = jpeg_std_error(&jerr);

	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);

	/* Step 2: specify data source (eg, a file) */

	jpeg_stdio_src(&cinfo, fbuffer);

	/* Step 3: read file parameters with jpeg_read_header() */

	(void)jpeg_read_header(&cinfo, TRUE);
	/* We can ignore the return value from jpeg_read_header since
	*   (a) suspension is not possible with the stdio data source, and
	*   (b) we passed TRUE to reject a tables-only JPEG file as an error.
	* See libjpeg.doc for more info.
	*/

	/* Step 4: set parameters for decompression */

	/* In this example, we don't need to change any of the defaults set by
	* jpeg_read_header(), so we do nothing here.
	*/

	/* Step 5: Start decompressor */

	(void)jpeg_start_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	* with the stdio data source.
	*/

	/* We may need to do some setup of our own at this point before reading
	* the data.  After jpeg_start_decompress() we have the correct scaled
	* output image dimensions available, as well as the output colormap
	* if we asked for color quantization.
	* In this example, we need to make an output work buffer of the right size.
	*/
	/* JSAMPLEs per row in output buffer */
	pixelcount = cinfo.output_width * cinfo.output_height;
	row_stride = cinfo.output_width * cinfo.output_components;
	out = (unsigned char *)malloc(pixelcount * 4);

	if(!cinfo.output_width || !cinfo.output_height
		|| ((pixelcount * 4) / cinfo.output_width) / 4 != cinfo.output_height
		|| pixelcount > 0x1FFFFFFF || cinfo.output_components > 4) // 4*1FFFFFFF == 0x7FFFFFFC < 0x7FFFFFFF
	{
		return 0;
	}
	
	RGBAImage* image = new RGBAImage(cinfo.output_width, cinfo.output_height);
	out = image->getRGBAPixels();

	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */

	/* Here we use the library's state variable cinfo.output_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	*/
	while(cinfo.output_scanline < cinfo.output_height)
	{
		/* jpeg_read_scanlines expects an array of pointers to scanlines.
		* Here the array is only one element long, but you could ask for
		* more than one scanline at a time if that's more convenient.
		*/
		bbuf = ((out + (row_stride * cinfo.output_scanline)));
		buffer = &bbuf;
		(void)jpeg_read_scanlines(&cinfo, buffer, 1);
	}

	// If we are processing an 8-bit JPEG (greyscale), we'll have to convert
	// the greyscale values to RGBA.
	if(cinfo.output_components == 1)
	{
		int				sindex, dindex = 0;
		unsigned char	greyshade;
  	 
		// allocate a new buffer for the transformed image
		out_converted = (unsigned char *)malloc(pixelcount*4);
  	 
		for(sindex = 0; sindex < pixelcount; sindex++)
		{
			greyshade = out[sindex];
			out_converted[dindex++] = greyshade;
			out_converted[dindex++] = greyshade;
			out_converted[dindex++] = greyshade;
			out_converted[dindex++] = 255;
		}
  	 
		free(out);
		out = out_converted;
	}
	else
	{
		// clear all the alphas to 255
		int             i, j;
		byte           *buf;

		buf = out;

		j = cinfo.output_width * cinfo.output_height * 4;
		for(i = 3; i < j; i += 4)
		{
			buf[i] = 255;
		}
	}

	/* Step 7: Finish decompression */

	(void)jpeg_finish_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	* with the stdio data source.
	*/

	/* Step 8: Release JPEG decompression object */

	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_decompress(&cinfo);

	/* After finish_decompress, we can close the input file.
	* Here we postpone it until after no more JPEG errors are possible,
	* so as to simplify the setjmp error logic above.  (Actually, I don't
	* think that jpeg_destroy can do an error exit, but why assume anything...)
	*/
//	free(fbuffer);

	/* At this point you may want to check to see whether any corrupt-data
	* warnings occurred (test whether jerr.pub.num_warnings is nonzero).
	*/

	/* And we're done! */
	return image;
}

Image* LoadJPG(ArchiveFile& file)
{
  ScopedArchiveBuffer buffer(file);
  return LoadJPGBuffer(buffer.buffer);
}


#include "modulesystem/singletonmodule.h"


class ImageDependencies : public GlobalFileSystemModuleRef
{
};

class ImageJPGAPI
{
  _QERPlugImageTable m_imagejpg;
public:
  typedef _QERPlugImageTable Type;
  STRING_CONSTANT(Name, "jpg");

  ImageJPGAPI()
  {
    m_imagejpg.loadImage = LoadJPG;
  }
  _QERPlugImageTable* getTable()
  {
    return &m_imagejpg;
  }
};

typedef SingletonModule<ImageJPGAPI, ImageDependencies> ImageJPGModule;

ImageJPGModule g_ImageJPGModule;


extern "C" void RADIANT_DLLEXPORT Radiant_RegisterModules(ModuleServer& server)
{
  initialiseModule(server);

  g_ImageJPGModule.selfRegister();
}

