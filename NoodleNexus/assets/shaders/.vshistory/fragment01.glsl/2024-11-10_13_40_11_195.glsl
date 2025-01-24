#version 330
in vec3 fColour;            // 3D model color from vertex buffer
in vec4 fvertexWorldLocation;
in vec4 fvertexNormal;

uniform vec4 objectColour;           // Override color 
uniform bool bUseObjectColour;
uniform vec4 eyeLocation;            // Camera position
uniform bool bDoNotLight;            // Skip lighting if true

out vec4 finalPixelColour;

const int POINT_LIGHT_TYPE = 0;
const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;

struct sLight
{
    vec4 position;         
    vec4 diffuse;   
    vec4 specular;  
    vec4 atten;     
    vec4 direction; 
    vec4 param1;    
    vec4 param2;    
};

const int NUMBEROFLIGHTS = 100;
uniform sLight theLights[NUMBEROFLIGHTS]; 

vec4 calculateLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular );

void main()
{
    vec3 vertexColour = fColour;
    if ( bUseObjectColour )
    {
        vertexColour = objectColour.rgb;
    }
    
    // Use lighting?
    if ( bDoNotLight )
    {
        finalPixelColour.rgb = objectColour.rgb;
        finalPixelColour.a = 1.0f;
        return;
    }
    
    vec4 vertexSpecular = vec4(1.0f, 1.0f, 1.0f, 1.0f);    

    vec4 pixelColour = calculateLightContrib( vertexColour.rgb, 
                                              fvertexNormal.xyz, 
                                              fvertexWorldLocation.xyz, 
                                              vertexSpecular );

    finalPixelColour = pixelColour;
    finalPixelColour.a = 1.0f;        
}

// Inspired by Mike Bailey's Graphic Shader, chapter 6
vec4 calculateLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular )
{
    vec3 norm = normalize(vertexNormal);
    
    vec4 finalObjectColour = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
    
    for ( int index = 0; index < NUMBEROFLIGHTS; index++ )
    {   
        if ( theLights[index].param2.x == 0.0f )
        {   
            continue;
        }
        
        int intLightType = int(theLights[index].param1.x);
        
        if ( intLightType == DIRECTIONAL_LIGHT_TYPE )
        {
            vec3 lightContrib = theLights[index].diffuse.rgb;
            float dotProduct = dot( -theLights[index].direction.xyz, normalize(norm.xyz) );  
            dotProduct = max( 0.0f, dotProduct );     
            lightContrib *= dotProduct;      
            finalObjectColour.rgb += (vertexMaterialColour.rgb * theLights[index].diffuse.rgb * lightContrib); 
            return finalObjectColour;        
        }
        
        vec3 vLightToVertex = theLights[index].position.xyz - vertexWorldPos.xyz;
        float distanceToLight = length(vLightToVertex);   
        vec3 lightVector = normalize(vLightToVertex);
        float dotProduct = dot(lightVector, vertexNormal.xyz);  
        
        if ( distanceToLight > theLights[index].atten.w )
        {
            finalObjectColour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
            return finalObjectColour;
        }
        
        dotProduct = max( 0.0f, dotProduct );    
        
        vec3 lightDiffuseContrib = dotProduct * theLights[index].diffuse.rgb;

        vec3 lightSpecularContrib = vec3(0.0f);
        vec3 reflectVector = reflect( -lightVector, normalize(norm.xyz) );

        vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);
        float objectSpecularPower = vertexSpecular.w; 
        
        lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
                               * theLights[index].specular.rgb;
                               
        float attenuation = 1.0f / 
                ( theLights[index].atten.x +                                         
                  theLights[index].atten.y * distanceToLight +                        
                  theLights[index].atten.z * distanceToLight*distanceToLight );     
                  
        lightDiffuseContrib *= attenuation;
        lightSpecularContrib *= attenuation;
        
        if ( intLightType == SPOT_LIGHT_TYPE )
        {   
            vec3 vertexToLight = vertexWorldPos.xyz - theLights[index].position.xyz;
            vertexToLight = normalize(vertexToLight);

            float currentLightRayAngle = dot( vertexToLight.xyz, theLights[index].direction.xyz );
            currentLightRayAngle = max(0.0f, currentLightRayAngle);

            float outerConeAngleCos = cos(radians(theLights[index].param1.z));
            float innerConeAngleCos = cos(radians(theLights[index].param1.y));
                            
            if ( currentLightRayAngle < outerConeAngleCos )
            {
                lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
                lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
            }
            else if ( currentLightRayAngle < innerConeAngleCos )
            {
                float penumbraRatio = (currentLightRayAngle - outerConeAngleCos) / 
                                      (innerConeAngleCos - outerConeAngleCos);
                                      
                lightDiffuseContrib *= penumbraRatio;
                lightSpecularContrib *= penumbraRatio;
            }        
        }
        
        finalObjectColour.rgb += (vertexMaterialColour.rgb * lightDiffuseContrib.rgb)
                                  + (vertexSpecular.rgb  * lightSpecularContrib.rgb );

    }
    
    finalObjectColour.a = 1.0f;
    
    return finalObjectColour;
}
