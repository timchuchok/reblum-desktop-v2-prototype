#include "Renderer.h"

#include <QFile>

// ── Lifecycle
// ─────────────────────────────────────────────────────────────────

Renderer::~Renderer() { releaseResources(); }

void Renderer::initialize(QRhi* rhi, QRhiRenderTarget* rt) {
    if (m_rhi != rhi) {
        releaseResources();
        m_rhi = rhi;
    }
    if (m_pipeline) {
        return;
    }

    m_vertUBO = m_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer,
                                 sizeof(VertexUBO));
    m_vertUBO->create();

    m_fragUBO = m_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer,
                                 sizeof(FragUBO));
    m_fragUBO->create();

    m_texture = m_rhi->newTexture(QRhiTexture::RGBA8, QSize(1, 1), 1);
    m_texture->create();

    m_sampler = m_rhi->newSampler(QRhiSampler::Linear, QRhiSampler::Linear,
                                  QRhiSampler::None, QRhiSampler::ClampToEdge,
                                  QRhiSampler::ClampToEdge);
    m_sampler->create();

    rebuildSRB();

    const QShader vs =
        loadShader(":/shaders/assets/shaders/fullscreen.vert.qsb");
    const QShader fs = loadShader(":/shaders/assets/shaders/image.frag.qsb");

    m_pipeline = m_rhi->newGraphicsPipeline();
    m_pipeline->setTopology(QRhiGraphicsPipeline::TriangleStrip);
    m_pipeline->setShaderStages(
        {{QRhiShaderStage::Vertex, vs}, {QRhiShaderStage::Fragment, fs}});
    m_pipeline->setVertexInputLayout({});
    m_pipeline->setShaderResourceBindings(m_srb);
    m_pipeline->setRenderPassDescriptor(rt->renderPassDescriptor());
    m_pipeline->create();

    m_effectsDirty = true;
    if (!m_pendingImage.isNull()) {
        m_textureDirty = true;
    }
}

void Renderer::releaseResources() {
    delete m_pipeline;
    m_pipeline = nullptr;
    delete m_srb;
    m_srb = nullptr;
    delete m_sampler;
    m_sampler = nullptr;
    delete m_texture;
    m_texture = nullptr;
    delete m_fragUBO;
    m_fragUBO = nullptr;
    delete m_vertUBO;
    m_vertUBO = nullptr;
}

// ── Data setters
// ──────────────────────────────────────────────────────────────

void Renderer::setImage(const QImage& image) {
    m_pendingImage = image;
    m_textureDirty = true;
    m_hasImage = !image.isNull();
}

void Renderer::setEffects(const EffectSettings& orange,
                          const EffectSettings& green) {
    m_orange = orange;
    m_green = green;
    m_effectsDirty = true;
}

// ── Render
// ────────────────────────────────────────────────────────────────────

void Renderer::render(QRhiCommandBuffer* cb, QRhiRenderTarget* rt,
                      const QRectF& imageLogicalRect, float dpr) {
    QRhiResourceUpdateBatch* rub = m_rhi->nextResourceUpdateBatch();

    if (m_textureDirty && !m_pendingImage.isNull()) {
        if (m_texture->pixelSize() != m_pendingImage.size()) {
            delete m_texture;
            m_texture =
                m_rhi->newTexture(QRhiTexture::RGBA8, m_pendingImage.size(), 1);
            m_texture->create();
            rebuildSRB();
        }
        rub->uploadTexture(
            m_texture,
            QRhiTextureUploadDescription(QRhiTextureUploadEntry(
                0, 0,
                QRhiTextureSubresourceUploadDescription(m_pendingImage))));
        m_textureDirty = false;
    }

    const QSize physicalSize = rt->pixelSize();
    const VertexUBO vubo = buildVertexUBO(imageLogicalRect, physicalSize, dpr);
    rub->updateDynamicBuffer(m_vertUBO, 0, sizeof(VertexUBO), &vubo);

    if (m_effectsDirty) {
        const FragUBO fubo = buildFragUBO();
        rub->updateDynamicBuffer(m_fragUBO, 0, sizeof(FragUBO), &fubo);
        m_effectsDirty = false;
    }

    cb->beginPass(rt, QColor("#0a0a0a"), {1.0f, 0}, rub);

    if (m_hasImage) {
        cb->setGraphicsPipeline(m_pipeline);
        cb->setViewport(
            QRhiViewport(0, 0, physicalSize.width(), physicalSize.height()));
        cb->setShaderResources(m_srb);
        cb->draw(4);
    }

    cb->endPass();
}

// ── Private helpers
// ───────────────────────────────────────────────────────────

Renderer::VertexUBO Renderer::buildVertexUBO(const QRectF& imgLogicalRect,
                                             const QSize& physicalSize,
                                             float dpr) const {
    VertexUBO ubo{};
    const QMatrix4x4 corr = m_rhi->clipSpaceCorrMatrix();
    memcpy(ubo.corrMatrix, corr.constData(), sizeof(ubo.corrMatrix));

    if (!m_hasImage) {
        return ubo;
    }

    const float pw = static_cast<float>(physicalSize.width());
    const float ph = static_cast<float>(physicalSize.height());

    ubo.imgRect[0] = imgLogicalRect.left() * dpr / pw * 2.0f - 1.0f;
    ubo.imgRect[1] = 1.0f - imgLogicalRect.top() * dpr / ph * 2.0f;
    ubo.imgRect[2] = imgLogicalRect.right() * dpr / pw * 2.0f - 1.0f;
    ubo.imgRect[3] = 1.0f - imgLogicalRect.bottom() * dpr / ph * 2.0f;

    return ubo;
}

Renderer::FragUBO Renderer::buildFragUBO() const {
    FragUBO ubo{};
    ubo.orangeOpacity = m_orange.opacity;
    ubo.orangeThreshold = m_orange.threshold;
    ubo.greenOpacity = m_green.opacity;
    ubo.greenThreshold = m_green.threshold;
    ubo.orangeEnabled = m_orange.enabled ? 1 : 0;
    ubo.greenEnabled = m_green.enabled ? 1 : 0;

    const auto toF = [](const QColor& c, float out[4]) {
        out[0] = static_cast<float>(c.redF());
        out[1] = static_cast<float>(c.greenF());
        out[2] = static_cast<float>(c.blueF());
        out[3] = 1.0f;
    };
    toF(m_orange.color, ubo.orangeColor);
    toF(m_green.color, ubo.greenColor);

    return ubo;
}

void Renderer::rebuildSRB() {
    delete m_srb;
    m_srb = m_rhi->newShaderResourceBindings();
    m_srb->setBindings({
        QRhiShaderResourceBinding::uniformBuffer(
            0, QRhiShaderResourceBinding::VertexStage, m_vertUBO),
        QRhiShaderResourceBinding::sampledTexture(
            1, QRhiShaderResourceBinding::FragmentStage, m_texture, m_sampler),
        QRhiShaderResourceBinding::uniformBuffer(
            2, QRhiShaderResourceBinding::FragmentStage, m_fragUBO),
    });
    m_srb->create();
}

QShader Renderer::loadShader(const QString& path) {
    QFile f(path);
    return f.open(QIODevice::ReadOnly) ? QShader::fromSerialized(f.readAll())
                                       : QShader{};
}
