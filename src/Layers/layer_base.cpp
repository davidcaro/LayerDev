#include "Layers\layer_base.h"
#include "GUI\SingleLayerGui.h"

Layer_base::Layer_base(string _name, Layer_Manager * layer_manager) :
    name(_name),
    b_active(false),
    layer_manager(layer_manager)
{
    quadSetup();
}

Layer_base::~Layer_base()
{
    onDestroy();
}

void Layer_base::setup(int  _width, int _height) {
    size = glm::vec2(_width, _height);
    setupFbo(size.x, size.y);

    onSetup();

    p_reset.addListener(this, &Layer_base::onResetInternal);
    p_reset.set("Reset", false);
    p_disable.set("Disable", false);
    p_debugRedraw.set("Redraw", false);
    params.add(
        p_reset,
        p_disable,
        p_debugRedraw
    );

    l_paramsChanged = params.parameterChangedE().newListener([&](ofAbstractParameter &) {return this->redraw(); }, OF_EVENT_ORDER_BEFORE_APP);

    onSetupParams();
}

void Layer_base::drawGui() 
{
    SingleLayerGui::drawGui(this); 
    onDrawGui();
}

void Layer_base::registerType(const string & name, Layer_factory * factory)
{
    (Layer_base::GetFactoryDirectory())->insert_or_assign(name, factory);
}

Layer_base * Layer_base::create(const string & name, Layer_Manager * _layer_manager )
{
    return (Layer_base::GetFactoryDirectory())->at(name)->create(_layer_manager);
}

vector<string> Layer_base::get_layer_names()
{
    vector<string> names;
    names.reserve((Layer_base::GetFactoryDirectory())->size());

    for (auto factory : *(Layer_base::GetFactoryDirectory())) {
        names.push_back(factory.first);
    }

    return names;
}

void Layer_base::setupFbo(int w, int h)
{  
    ofFboSettings settings;
    settings.numColorbuffers    = 1;   // one buff per fbo
    settings.width              = w;
    settings.height             = h;
    settings.internalformat     = GL_RGBA;
    settings.wrapModeHorizontal = true;
    settings.wrapModeVertical   = true;
    settings.numSamples         = 0;
    settings.useDepth           = false;
    settings.useStencil         = false;
    settings.textureTarget      = GL_TEXTURE_2D;
    fbo.allocate(settings);
    clearFbo();
}

void Layer_base::resize(int _width, int _height )
{
    size = glm::vec2(_width, _height);
    setupFbo(size.x, size.y);
    onResize();
    redraw();
}

void Layer_base::clearFbo() const
{
    fbo.begin();
    ofClear(0);
    fbo.end();
}

void Layer_base::onResetInternal(bool & b_reset) {
    if (b_reset) {
        reset();
        b_reset = false;
    }
}

bool Static_base::draw(pingPongFbo & mainFbo, bool _forceRedraw) const
{
    if (p_disable) {
        return needsRedraw();
    }
    else if (needsRedraw() || _forceRedraw) {
        fbo.begin();
        ofClear(0);
        onDraw();
        fbo.end();

        mainFbo.begin();
        fbo.draw(0, 0);
        mainFbo.end();
        setRedraw(false);
        return REDRAW;
    } else {            
        mainFbo.begin();
        fbo.draw(0, 0);
        mainFbo.end();
        return NO_REDRAW;
    }
}

bool Filter_base::draw(pingPongFbo & mainFbo, bool _forceRedraw) const
{
    if (p_disable ) {
        return needsRedraw();
    } else if (needsRedraw() || _forceRedraw) {
        mainFbo.swap();

        fbo.begin();
        ofClear(0);
        onDraw(mainFbo.getBackTexture());
        fbo.end();

        setRedraw(false);
        
        mainFbo.begin();
        fbo.draw(0, 0);
        mainFbo.end();

        return REDRAW;
    }
    else {
        mainFbo.begin();
        fbo.draw(0, 0);
        mainFbo.end();
        return NO_REDRAW;
    }
}

void Layer_base::quadSetup()
{
    base_shader = Shader_lib::get_passthrough_shader();

    baseQuad.addVertex(glm::vec3(0));
	baseQuad.addVertex(glm::vec3(0));
	baseQuad.addVertex(glm::vec3(0));
	baseQuad.addVertex(glm::vec3(0));
			
	baseQuad.addTexCoord(glm::vec2(0.0, 0.0));
	baseQuad.addTexCoord(glm::vec2(1.0, 0.0));
	baseQuad.addTexCoord(glm::vec2(1.0, 1.0));
	baseQuad.addTexCoord(glm::vec2(0.0, 1.0));

    baseQuad.addIndex(0);
    baseQuad.addIndex(1);
    baseQuad.addIndex(2);

    baseQuad.addIndex(2);
    baseQuad.addIndex(3);
    baseQuad.addIndex(0);
}

void Layer_base::setQuad(const ofTexture & _baseTex) const
{
    baseQuad.setVertex(0, glm::vec3( 0                  , 0                   , 0 ));
	baseQuad.setVertex(1, glm::vec3( _baseTex.getWidth(), 0                   , 0 ));
	baseQuad.setVertex(2, glm::vec3( _baseTex.getWidth(), _baseTex.getHeight(), 0 ));
	baseQuad.setVertex(3, glm::vec3( 0                  , _baseTex.getHeight(), 0 ));
}

void Layer_base::drawTexture(const ofTexture & _baseTex) const
{
    setQuad(_baseTex);
    base_shader->begin();
    _baseTex.bind();
    baseQuad.draw();
    base_shader->end();
}