#ifndef CIMAGE_PIXEL_RENDERER_H
#define CIMAGE_PIXEL_RENDERER_H

#include <CPixelRenderer.h>

class CImagePixelRenderer : public CPixelRenderer {
 public:
  explicit CImagePixelRenderer(int width=100, int height=100);
  explicit CImagePixelRenderer(CImagePtr image);

  virtual ~CImagePixelRenderer();

  CImagePixelRenderer(const CImagePixelRenderer &) = delete;
  CImagePixelRenderer &operator=(const CImagePixelRenderer &) = delete;

  CImagePixelRenderer *dup() const override;

  CImagePtr getImage() override { return image_; }
  void setImage(CImagePtr image) override;

  uint getWidth() const override;
  uint getHeight() const override;

  void updateSize(int width, int height) override;

  void clear() override;
  void fill() override;

  void drawClippedPoint(const CIPoint2D &point) override;

  void drawClippedImage(const CIPoint2D &point, CImagePtr image) override;

  virtual void writePNG(const std::string &filename);

 private:
  CImagePtr image_;
};

#endif
