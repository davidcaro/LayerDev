#include "Layers\layer_fliter_mpeg_glitch.h"

REGISTER_TYPE(layer_fliter_mpeg_glitch)

void layer_fliter_mpeg_glitch::onSetup()
{
    Layer_filter_shader::onSetup();

    ofPixels noisePixels;
    noiseTex.allocate(noiseTexSize, noiseTexSize, GL_RGB);
    noiseTex.setTextureWrap(GL_REPEAT, GL_REPEAT);
    noisePixels.allocate(noiseTexSize, noiseTexSize, OF_PIXELS_RGB);

    for (int x = 0; x < noiseTexSize; x++) {
        for (int y = 0; y < noiseTexSize; y++) {
            noisePixels.setColor(x, y, ofColor(ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255)));
        }
    }
    noiseTex.loadData(noisePixels);    
}

void layer_fliter_mpeg_glitch::onUpdate()
{
    if (!p_pause) redraw();
}

void layer_fliter_mpeg_glitch::onSetupParams()
{
    p_blockSize.set("Block Size", 16, 8, 64);
    p_pause.set("Pause"  , false);

    params.add(
        p_blockSize,
        p_pause
    );

}

void layer_fliter_mpeg_glitch::setupShader()
{
    shader = Shader_lib::get_mpeg_glitch_shader();
}

void layer_fliter_mpeg_glitch::setUniforms(const ofTexture & _baseTex) const
{
    shader->setUniformTexture("u_imageTex", _baseTex, 0);
    shader->setUniformTexture("u_noiseTex", noiseTex, 1);
    shader->setUniform2f("u_resolution", noiseTex.getWidth(), noiseTex.getHeight());
    shader->setUniform1f("u_blockSize", p_blockSize);
    shader->setUniform1f("u_time", ofGetSystemTimeMillis());
}