#pragma once

#include <rhi/qrhi.h>

#include <QImage>

#include "core/effects/EffectSettings.h"

class Renderer {
public:
    Renderer() = default;
    ~Renderer();

    void initialize(QRhi* rhi, QRhiRenderTarget* rt);
    void releaseResources();

    void setImage(const QImage& image);
    void setEffects(const EffectSettings& orange, const EffectSettings& green);

    // imageLogicalRect: image rect in logical (pre-DPR) widget pixels
    void render(QRhiCommandBuffer* cb, QRhiRenderTarget* rt,
                const QRectF& imageLogicalRect, float dpr);

    bool hasImage() const { return m_hasImage; }

private:
    // Must match std140 layout in shaders
    struct alignas(16) VertexUBO {
        float corrMatrix[16];  // mat4, offset 0
        float imgRect[4];      // vec4, offset 64: left,top,right,bottom (NDC)
    };
    struct FragUBO {
        float orangeOpacity;    // offset 0
        float orangeThreshold;  // offset 4
        float greenOpacity;     // offset 8
        float greenThreshold;   // offset 12
        int32_t orangeEnabled;  // offset 16
        int32_t greenEnabled;   // offset 20
        float _pad[2];          // offset 24 — padding before vec4
        float orangeColor[4];   // offset 32 (vec4, alignment 16)
        float greenColor[4];    // offset 48
    };                          // total 64 bytes

    VertexUBO buildVertexUBO(const QRectF& imgLogicalRect,
                             const QSize& physicalSize, float dpr) const;
    FragUBO buildFragUBO() const;
    void rebuildSRB();
    static QShader loadShader(const QString& path);

    QRhi* m_rhi = nullptr;
    QRhiBuffer* m_vertUBO = nullptr;
    QRhiBuffer* m_fragUBO = nullptr;
    QRhiTexture* m_texture = nullptr;
    QRhiSampler* m_sampler = nullptr;
    QRhiShaderResourceBindings* m_srb = nullptr;
    QRhiGraphicsPipeline* m_pipeline = nullptr;

    EffectSettings m_orange;
    EffectSettings m_green;

    QImage m_pendingImage;
    bool m_textureDirty = false;
    bool m_effectsDirty = true;
    bool m_hasImage = false;
};
