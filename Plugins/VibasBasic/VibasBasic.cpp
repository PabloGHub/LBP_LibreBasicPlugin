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
#include "ofxMessage.h"

#include <iostream>
#include <fstream>

//#include <cmath>

/*
--OBJETIVO: Que la imagen a la que se le aplique el efecto vibre aleatoriamente a partir de unos parametros.

    NOTA: Todo los textos tinen que estar: "Español/Ingles".
	TODO: (Corregir) Si bajas mucho la frecuencia va ah tirones, Pero se soluciona al subir la amplitud.
*/


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ppal                                                                                                             //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
using namespace OFX;
class VibraBasicPlugin : public OFX::ImageEffect
{
    public: VibraBasicPlugin(OfxImageEffectHandle handle) : ImageEffect(handle), _previoOffsetX(0), _previoOffsetY(0)
    {
        _amplitud = fetchDoubleParam("Amplitud/Amplitude");
		_frecuencia = fetchDoubleParam("Frecuencia/Frequency");
		_inicial = fetchDoubleParam("Inicio/Starting");
        _modo = fetchChoiceParam("Modo/Mode");
		_aDonde = fetchChoiceParam("Donde/Where");
		crearConsola();
    }

    virtual void render(const OFX::RenderArguments& args) override
    {
        // Obtener los parámetros de vibración
        double amplitude =      _amplitud->getValueAtTime(args.time);
        double frequency =      _frecuencia->getValueAtTime(args.time);
        double initialPoint =   _inicial->getValueAtTime(args.time);
        int modo;   /* = */    _modo->getValueAtTime(args.time, modo);
		int aDonde; /* = */    _aDonde->getValueAtTime(args.time, aDonde);
        double tiempo = args.time * frequency; //fmod(args.time, 2 * 3.14159265359 / frequency);
        
		std::cout << "MQM" << std::endl;

        // Obtener la imagen de entrada
        OFX::Clip* srcClip = fetchClip("Source");
        OFX::Image* srcImg = srcClip->fetchImage(args.time);

        // Obtener la imagen de salida
        OFX::Clip* dstClip = fetchClip("Output");
        OFX::Image* dstImg = dstClip->fetchImage(args.time);

        // Comprobar si se le aplicar al "transform" de la propia imagen.
        if (aDonde == 0)
        {
            // Aplicar el efecto de vibración
            if (srcImg && dstImg)
            {
                OfxRectI srcBounds = srcImg->getBounds();
                OfxRectI dstBounds = dstImg->getBounds();

                for (int y = srcBounds.y1; y < srcBounds.y2; ++y)
                {
                    for (int x = srcBounds.x1; x < srcBounds.x2; ++x)
                    {
                        // Calcular el desplazamiento aleatorio
                        int offsetX = 0;
                        int offsetY = 0;

                        // O = Circular, | = Vertical, - = Horizontal, / = Diagonal, \ = Diagonal Inversa
                        switch (modo)
                        {
                            case 0: // Circular
                                offsetX = static_cast<int>(amplitude * sin(tiempo + initialPoint));
                                offsetY = static_cast<int>(amplitude * cos(tiempo + initialPoint));
                            break;

                            case 1: // Vertical
                                offsetY = static_cast<int>(amplitude * sin(tiempo + initialPoint));
                            break;

                            case 2: // Horizontal
                                offsetX = static_cast<int>(amplitude * cos(tiempo + initialPoint));
                            break;

                            case 3: // Diagonal
                                offsetX = static_cast<int>(amplitude * sin(tiempo + initialPoint));
                                offsetY = static_cast<int>(amplitude * sin(tiempo + initialPoint));
                            break;

                            case 4: // Diagonal Inversa
                                offsetX = static_cast<int>(amplitude * sin(tiempo + initialPoint));
                                offsetY = static_cast<int>(-amplitude * sin(tiempo + initialPoint));
                            break;

                            case 5: // Aleatorio
                                // TODO: Implementar
                            break;
                        }

                        // Suavizar los desplazamientos acumulándolos
                        offsetX = static_cast<int>((_previoOffsetX + offsetX) * 0.5);
                        offsetY = static_cast<int>((_previoOffsetY + offsetY) * 0.5);

                        // Asegurarse de que los desplazamientos estén dentro de los límites
                        int srcX = x + offsetX;
                        int srcY = y + offsetY;

                        if (srcX >= srcBounds.x1 && srcX < srcBounds.x2 && srcY >= srcBounds.y1 && srcY < srcBounds.y2)
                        {
                            float* dstPixel = static_cast<float*>(dstImg->getPixelAddress(x, y));
                            float* srcPixel = static_cast<float*>(srcImg->getPixelAddress(srcX, srcY));

                            dstPixel[0] = srcPixel[0];
                            dstPixel[1] = srcPixel[1];
                            dstPixel[2] = srcPixel[2];
                            dstPixel[3] = srcPixel[3];
                        }

                        _previoOffsetX = offsetX;
                        _previoOffsetY = offsetY;
                    }
                }
            }

        }
        // Comrpobar si le afecta al efecto de "Imagen dentro de Imagen".
        else if (aDonde == 1)
        {
            // Verificar si el clip tiene el efecto "Picture in Picture"
            PropertySet& clipPropiedades = srcClip->getPropertySet();
            std::string nombreEfecto = clipPropiedades.propGetString("OfxImageEffectPropName");

			std::cout << "Nombre del efecto: " << nombreEfecto << std::endl;

            if (nombreEfecto == "Picture in Picture" || nombreEfecto == "PIP")
            {
                // Aplicar el efecto de vibración
                if (srcImg && dstImg)
                {
                    OfxRectI srcBounds = srcImg->getBounds();
                    OfxRectI dstBounds = dstImg->getBounds();

                    for (int y = srcBounds.y1; y < srcBounds.y2; ++y)
                    {
                        for (int x = srcBounds.x1; x < srcBounds.x2; ++x)
                        {
                            // Calcular el desplazamiento aleatorio
                            int offsetX = 0;
                            int offsetY = 0;

                            // O = Circular, | = Vertical, - = Horizontal, / = Diagonal, \ = Diagonal Inversa
                            switch (modo)
                            {
                            case 0: // Circular
                                offsetX = static_cast<int>(amplitude * sin(tiempo + initialPoint));
                                offsetY = static_cast<int>(amplitude * cos(tiempo + initialPoint));
                                break;

                            case 1: // Vertical
                                offsetY = static_cast<int>(amplitude * sin(tiempo + initialPoint));
                                break;

                            case 2: // Horizontal
                                offsetX = static_cast<int>(amplitude * cos(tiempo + initialPoint));
                                break;

                            case 3: // Diagonal
                                offsetX = static_cast<int>(amplitude * sin(tiempo + initialPoint));
                                offsetY = static_cast<int>(amplitude * sin(tiempo + initialPoint));
                                break;

                            case 4: // Diagonal Inversa
                                offsetX = static_cast<int>(amplitude * sin(tiempo + initialPoint));
                                offsetY = static_cast<int>(-amplitude * sin(tiempo + initialPoint));
                                break;

                            case 5: // Aleatorio
                                // TODO: Implementar
                                break;
                            }

                            // Suavizar los desplazamientos acumulándolos
                            offsetX = static_cast<int>((_previoOffsetX + offsetX) * 0.5);
                            offsetY = static_cast<int>((_previoOffsetY + offsetY) * 0.5);

                            // Asegurarse de que los desplazamientos estén dentro de los límites
                            int srcX = x + offsetX;
                            int srcY = y + offsetY;

                            if (srcX >= srcBounds.x1 && srcX < srcBounds.x2 && srcY >= srcBounds.y1 && srcY < srcBounds.y2)
                            {
                                float* dstPixel = static_cast<float*>(dstImg->getPixelAddress(x, y));
                                float* srcPixel = static_cast<float*>(srcImg->getPixelAddress(srcX, srcY));

                                dstPixel[0] = srcPixel[0];
                                dstPixel[1] = srcPixel[1];
                                dstPixel[2] = srcPixel[2];
                                dstPixel[3] = srcPixel[3];
                            }

                            _previoOffsetX = offsetX;
                            _previoOffsetY = offsetY;
                        }
                    }
                }
            }
        }
       

    }

    void crearConsola()
    {
        AllocConsole();

        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONIN$", "r", stdin);
        freopen_s(&fp, "CONOUT$", "w", stderr);

        std::cout << "Consola de debug inicializada." << std::endl;
    }

    virtual void getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences) override
    {
        clipPreferences.setOutputFrameVarying(true);
    }

    private:
        OFX::DoubleParam* _amplitud;
        OFX::DoubleParam* _frecuencia;
        OFX::DoubleParam* _inicial;
        OFX::ChoiceParam* _modo;
		OFX::ChoiceParam* _aDonde;
		int _previoOffsetX;
		int _previoOffsetY;
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Definiciones/Apartados                                                                                           //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
mDeclarePluginFactory(VibraBasicPluginFactoria, {}, {});
void VibraBasicPluginFactoria::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("Vibracion/shaking", "Vibracion/shaking", "Vibracion/shaking");
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


    //Modo
    ChoiceParamDescriptor* modoParam = desc.defineChoiceParam("Modo/Mode");
    modoParam->setLabels("Modo/Mode", "Modo/Mode", "Modo/Mode");
    std::string _hint = "";
    _hint += "Selecciona el modo de vibración/Select the vibration mode\n";
	_hint += "Español: O = Circular, | = Vertical, - = Horizontal, / = Diagonal, \\ = Diagonal Inversa\n";
	_hint += "English: O = Circular, | = Vertical, - = Horizontal, / = Diagonal, \\ = Inverse Diagonal";
    modoParam->setHint(_hint);
    modoParam->appendOption("O");
    modoParam->appendOption("|");
    modoParam->appendOption("-");
    modoParam->appendOption("/");
    modoParam->appendOption("\\");
	modoParam->appendOption("Aleatorio/Random");
    modoParam->setDefault(0);
    page->addChild(*modoParam);

    // Añadir parámetros de vibración
	DoubleParamDescriptor* amplitudeParam = desc.defineDoubleParam("Amplitud/Amplitude");
    amplitudeParam->setLabels("Amplitud/Amplitude", "Amplitud/Amplitude", "Amplitud/Amplitude");
	amplitudeParam->setHint("Amplitud de la vibración/Amplitude of the vibration");
    amplitudeParam->setDefault(5.0);
	amplitudeParam->setDisplayRange(2.00, 20.0);
    amplitudeParam->setAnimates(true);
    page->addChild(*amplitudeParam);

	DoubleParamDescriptor* frequencyParam = desc.defineDoubleParam("Frecuencia/Frequency");
    frequencyParam->setLabels("Frecuencia/Frequency", "Frecuencia/Frequency", "Frecuencia/Frequency");
	frequencyParam->setHint("Frecuencia de la vibración/Frequency of the vibration");
    frequencyParam->setDefault(1.0);
    frequencyParam->setAnimates(true);
    page->addChild(*frequencyParam);

	DoubleParamDescriptor* initialPointParam = desc.defineDoubleParam("Inicio/Starting");
    initialPointParam->setLabels("Inicio/Starting", "Inicio/Starting", "Inicio/Starting");
	initialPointParam->setHint("Punto inicial del ciclo/Starting point of the cycle");
    initialPointParam->setDefault(0.0);
    initialPointParam->setAnimates(true);
	initialPointParam->setRange(-3.14159265359, 3.14159265359);
    initialPointParam->setDisplayRange(-3.14159265359, 3.14159265359);
	//initialPointParam->setInteractDescriptor(new OFX::ParamInteractDescriptor());
    page->addChild(*initialPointParam);


    // Panoramización o ImagenEnImagen -> Es lo que modificaremos
	ChoiceParamDescriptor* aDonde = desc.defineChoiceParam("Donde/Where");
	aDonde->setLabels("Panoramización/Pan", "ImagenEnImagen/ImageInImage", "Panoramización/Pan");
	aDonde->setHint("Panoramización o ImagenEnImagen/Pan or ImageInImage");
	aDonde->appendOption("Panoramización/Pan");
	aDonde->appendOption("ImagenEnImagen/ImageInImage");
    aDonde->setDefault(0);
	page->addChild(*aDonde);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Insercion                                                                                                        //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
            static VibraBasicPluginFactoria p("net.sf.openfx:vibraBasicPlugin", 1, 0);
            ids.push_back(&p);

			//Crear consola de debug
            AllocConsole();

            FILE* fp;
            freopen_s(&fp, "CONOUT$", "w", stdout);
            freopen_s(&fp, "CONIN$", "r", stdin);
            freopen_s(&fp, "CONOUT$", "w", stderr);

            std::cout << "Consola de debug inicializada." << std::endl;
        }
    }
}