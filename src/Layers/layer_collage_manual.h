#pragma once
#include "ofMain.h"
#include "Layers\layer_collage.h"

class Layer_collage_manual : public Layer_collage
{
public:
    Layer_collage_manual(string name, Layer_Manager * _layer_manager) : Layer_collage(name,  _layer_manager) {}

protected:

    virtual void onActivate()    override ;
    virtual void onDeactivate()  override ;
    
    void onFileDragEvent(ofDragInfo & _fileInfo);

    void onMouseMoved(ofMouseEventArgs & _args);
    void onMousePressed(ofMouseEventArgs & _args);
    void onMouseScrolled(ofMouseEventArgs & _args);

    virtual void onModeViewing() override;
    virtual void onModeEditing() override;
};
