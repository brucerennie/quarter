/**************************************************************************\
 *
 *  This file is part of the SIM Quarter extension library for Coin.
 *  Copyright (C) 1998-2007 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License ("GPL") version 2
 *  as published by the Free Software Foundation.  See the file COPYING
 *  at the root directory of this source distribution for additional
 *  information about the GNU GPL.
 *
 *  For using SIM Quarter with software that can not be combined with
 *  the GNU GPL, and for taking advantage of the additional benefits of
 *  our support services, please contact Systems in Motion about acquiring
 *  a Coin Professional Edition License.
 *
 *  See <URL:http://www.coin3d.org/> for more information.
 *
 *  Systems in Motion AS, Bygdøy allé 5, N-0257 Oslo, NORWAY. (www.sim.no)
 *
\**************************************************************************/

#include "ImageReader.h"
#include <Inventor/SbImage.h>
#include <QtGui/QImage>

using namespace SIM::Coin3D::Quarter;

ImageReader::ImageReader(void)
{
#if COIN_MAJOR_VERSION >= 3
  SbImage::addReadImageCB(ImageReader::readImageCB, this);
#endif // COIN_MAJOR_VERSION >= 3
}

ImageReader::~ImageReader(void)
{
#if COIN_MAJOR_VERSION >= 3
  SbImage::removeReadImageCB(ImageReader::readImageCB, this);
#endif // COIN_MAJOR_VERSION >= 3
}

SbBool
ImageReader::readImage(const SbString & filename, SbImage * sbimage) const
{
  QImage image;
  if (image.load(filename.getString())) {
    int c;
    int w = image.width();
    int h = image.height();

    // Keep in 8-bits mode if that was what we read
    if (image.depth() == 8 && image.isGrayscale()) {
      c = 1;
    }
    else {
      // FIXME: consider if we should detect allGrayscale() and alpha (c = 2)
      c = image.hasAlphaChannel() ? 4 : 3;
      image = image.convertToFormat(image.hasAlphaChannel() ? 
                                    QImage::Format_ARGB32 : QImage::Format_RGB32);
    }

    SbVec2s size((short) w, (short) h);
    sbimage->setValue(size, c, NULL);
    unsigned char * buffer = sbimage->getValue(size, c);

    if (c == 1) {
      for (int i = 0; i < h; i++) {
        memcpy(buffer + i*w, image.scanLine(h-(i+1)), w);
      }
    }
    else { // (c == 3 || c == 4)
      QRgb * bits = (QRgb*) image.bits();
      for (int y = 0; y < h; y++) {
        unsigned char * line = &buffer[c*w*(h-(y+1))];
        for (int x = 0; x < w; x++) {
          *line++ = qRed(*bits);
          *line++ = qGreen(*bits);
          *line++ = qBlue(*bits);
          if (c == 4) {
            *line++ = qAlpha(*bits);
          }
          bits++;
        }
      }
    }    
    return TRUE;
  }
  return FALSE;
}

SbBool 
ImageReader::readImageCB(const SbString & filename, SbImage * image, void * closure)
{
  return ((ImageReader*)closure)->readImage(filename, image);
}