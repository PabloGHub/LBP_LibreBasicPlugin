#ifdef _WINDOWS
#include <windows.h>
#endif

#ifdef __APPLE__
#include <AGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <stdio.h>
#include "ofxsImageEffect.h"
#include "ofxsMultiThread.h"

#include "../include/ofxsProcessing.H"


/*
--OBJETIVO: Que la imagen a la que se le aplique el efecto vibre aleatoriamente a partir de unos parametros.
-- Vale, Orden que voy a intentar seguir.
--1: De arriba a abajo.
--2: Debajo tener los auxiliar.
*/

using namespace OFX;

class VibraBasicPlugin : public OFX::ImageEffect
{
    public : VibraBasicPlugin(OfxImageEffectHandle handle) : ImageEffect(handle)
    {
        // No necesitamos hacer nada en el constructor
    }

    virtual void render(const OFX::RenderArguments& args) override
    {
        // No necesitamos hacer nada en el render
    }


};

mDeclarePluginFactory(VibraBasicPluginFactoria, {}, {});

void VibraBasicPluginFactoria::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("Hola Mundo", "Hola Mundo", "Hola Mundo");
    desc.setPluginGrouping("OFX");
    desc.addSupportedContext(eContextFilter);
    desc.addSupportedBitDepth(eBitDepthUByte);
    desc.addSupportedBitDepth(eBitDepthUShort);
    desc.addSupportedBitDepth(eBitDepthFloat);
    desc.setSingleInstance(false);
    desc.setHostFrameThreading(false);
    desc.setSupportsMultiResolution(true);
    desc.setSupportsTiles(true);
    desc.setTemporalClipAccess(false);
    desc.setRenderTwiceAlways(false);
    desc.setSupportsMultipleClipPARs(false);
}

void VibraBasicPluginFactoria::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::ContextEnum context)
{
    ClipDescriptor* srcClip = desc.defineClip("Source");
    srcClip->addSupportedComponent(ePixelComponentRGBA);
    srcClip->addSupportedComponent(ePixelComponentAlpha);
    srcClip->setTemporalClipAccess(false);
    srcClip->setSupportsTiles(true);
    srcClip->setIsMask(false);

    ClipDescriptor* dstClip = desc.defineClip("Output");
    dstClip->addSupportedComponent(ePixelComponentRGBA);
    dstClip->addSupportedComponent(ePixelComponentAlpha);
    dstClip->setSupportsTiles(true);

    PageParamDescriptor* page = desc.definePageParam("Controls");

    // Añadir un parámetro de texto que muestre "Hola Mundo"
    StringParamDescriptor* textParam = desc.defineStringParam("HolaMundo");
    textParam->setLabels("Hola Mundo", "Hola Mundo", "Hola Mundo");
    textParam->setHint("Este es un mensaje de Hola Mundo");
    textParam->setDefault("Hola Mundo");
    textParam->setAnimates(false); // No se puede animar
    page->addChild(*textParam);
}

ImageEffect* VibraBasicPluginFactoria::createInstance(OfxImageEffectHandle handle, OFX::ContextEnum context)
{
    return new VibraBasicPlugin(handle);
}

namespace OFX
{
    namespace Plugin
    {
        void getPluginIDs(OFX::PluginFactoryArray& ids)
        {
            static VibraBasicPluginFactoria p("net.sf.openfx:holaMundoPlugin", 1, 0);
            ids.push_back(&p);
        }
    }
}