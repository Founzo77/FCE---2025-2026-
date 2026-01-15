#include <fge/render/data/Texture.hpp>

#include <fge/utility.hpp>

#include <iostream>

namespace fge
{
    void Texture::initialize(const string& texturePathFile)
    {
        m_image = {}; // reset

        std::wstring widePath(texturePathFile.begin(), texturePathFile.end());

        TexMetadata metadata;
        ScratchImage image;

        HRESULT hr = LoadFromWICFile(
            widePath.c_str(),
            WIC_FLAGS_FORCE_SRGB, // force en sRGB (classique pour textures de couleur)
            &metadata,
            image
        );

        if (FAILED(hr))
        {
            std::cerr << "Erreur : impossible de charger la texture : " 
                << texturePathFile << std::endl;
            // TO_DO Laiser seulement throwIfFailed
            throwIfFailed(hr, "Impossible to load the texture");
            return;
        }

        // Déplacer l'image chargée vers la texture
        m_image = std::move(image);
        m_metaData = std::move(metadata);
    }
}
