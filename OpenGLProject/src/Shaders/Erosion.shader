#shader compute
#version 460 core

layout(local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 1) buffer MapBuffer { float Data[]; } heightMap;
layout(std430, binding = 2) buffer RandomIndicesBuffer{ int Data[];   } indices;
layout(std430, binding = 3) buffer BrushIndicesBuffer { int Data[];   } brushIndices;
layout(std430, binding = 4) buffer BrushWeightsBuffer { float Data[]; } brushWeightIndices;

uniform int u_mapSize;
uniform int u_brushLength;
uniform int u_borderSize;

uniform int u_maxLifetime;
uniform float u_inertia;
uniform float u_sedimentCapacityFactor;
uniform float u_minSedimentCapacity;
uniform float u_depositSpeed;
uniform float u_erodeSpeed;

uniform float u_evaporateSpeed;
uniform float u_gravity;
uniform float u_startSpeed;
uniform float u_startWater;

vec3 CalculateHeightAndGradient(float posX, float posY)
{
    int coordX = int(posX);
    int coordY = int(posY);

    // Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
    float x = posX - coordX;
    float y = posY - coordY;

    // Calculate heights of the four nodes of the droplet's cell
    int nodeIndexNW = coordY * u_mapSize + coordX;
    float heightNW = heightMap.Data[nodeIndexNW];
    float heightNE = heightMap.Data[nodeIndexNW + 1];
    float heightSW = heightMap.Data[nodeIndexNW + u_mapSize];
    float heightSE = heightMap.Data[nodeIndexNW + u_mapSize + 1];

    // Calculate droplet's direction of flow with bilinear interpolation of height difference along the edges
    float gradientX = (heightNE - heightNW) * (1.0 - y) + (heightSE - heightSW) * y;
    float gradientY = (heightSW - heightNW) * (1.0 - x) + (heightSE - heightNE) * x;

    // Calculate height with bilinear interpolation of the heights of the nodes of the cell
    float height = heightNW * (1.0 - x) * (1.0 - y) + heightNE * x * (1.0 - y) + heightSW * (1.0 - x) * y + heightSE * x * y;

    return vec3(gradientX, gradientY, height);
}

void Erode(float sedimentCapacity, inout float sediment, float deltaHeight, int dropletIndex)
{
    // Erode a fraction of the droplet's current carry capacity.
    // Clamp the erosion to the change in height so that it doesn't dig a hole in the terrain behind the droplet
    float amountToErode = min((sedimentCapacity - sediment) * u_erodeSpeed, -deltaHeight);
    
    for (int i = 0; i < u_brushLength; i++) 
    {
        int erodeIndex = dropletIndex + brushIndices.Data[i];

        float weightedErodeAmount = amountToErode * brushWeightIndices.Data[i];
        float deltaSediment = (heightMap.Data[erodeIndex] < weightedErodeAmount) ? heightMap.Data[erodeIndex] : weightedErodeAmount;
        heightMap.Data[erodeIndex] -= deltaSediment;
        sediment += deltaSediment;
    }
}

void Deposit(float sedimentCapacity, inout float sediment, float deltaHeight, float cellOffsetX, float cellOffsetY, int dropletIndex)
{
    // If moving uphill (deltaHeight > 0) try fill up to the current height, otherwise deposit a fraction of the excess sediment
    float amountToDeposit = (deltaHeight > 0.0) ? min(deltaHeight, sediment) : (sediment - sedimentCapacity) * u_depositSpeed;
    
    sediment -= amountToDeposit;

    // Add the sediment to the four nodes of the current cell using bilinear interpolation
    heightMap.Data[dropletIndex] += amountToDeposit * (1.0 - cellOffsetX) * (1.0 - cellOffsetY);
    heightMap.Data[dropletIndex + 1] += amountToDeposit * cellOffsetX * (1.0 - cellOffsetY);
    heightMap.Data[dropletIndex + u_mapSize] += amountToDeposit * (1.0 - cellOffsetX) * cellOffsetY;
    heightMap.Data[dropletIndex + u_mapSize + 1] += amountToDeposit * cellOffsetX * cellOffsetY;
}

void main() 
{
    uint index = indices.Data[gl_GlobalInvocationID.x];

    float posX = float(index % u_mapSize);
    float posY = float(index / u_mapSize);
    float dirX = 0.0;
    float dirY = 0.0;
    float speed = u_startSpeed;
    float water = u_startWater;
    float sediment = 0.0;

    for (int lifetime = 0; lifetime < u_maxLifetime; lifetime++) 
    {
        // Calculate droplet's cell position 
        int nodeX = int(posX);
        int nodeY = int(posY);
        
        // Calculate droplet's array index
        int dropletIndex = nodeY * u_mapSize + nodeX;
        
        // Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
        float cellOffsetX = posX - nodeX;
        float cellOffsetY = posY - nodeY;

        // Calculate droplet's height and direction of flow with bilinear interpolation of surrounding heights
        vec3 heightAndGradient = CalculateHeightAndGradient(posX, posY);

        // Update the droplet's direction and position (move position 1 unit regardless of speed)
        dirX = (dirX * u_inertia - heightAndGradient.x * (1.0 - u_inertia));
        dirY = (dirY * u_inertia - heightAndGradient.y * (1.0 - u_inertia));
        
        // Normalize direction
        float len = max(0.01, sqrt(dirX * dirX + dirY * dirY));
        dirX /= len;
        dirY /= len;
        posX += dirX;
        posY += dirY;

        // Stop simulating droplet if it's not moving or has flowed over edge of map
        if ((dirX == 0.0 && dirY == 0.0) || posX < u_borderSize || posX > u_mapSize - u_borderSize || posY < u_borderSize || posY > u_mapSize - u_borderSize) 
        {
            break;
        }

        // Find the droplet's new height and calculate the deltaHeight
        float newHeight = CalculateHeightAndGradient(posX, posY).z;
        float deltaHeight = newHeight - heightAndGradient.z;

        // Calculate the droplet's sediment capacity (higher when moving fast down a slope and contains lots of water)
        float sedimentCapacity = max(-deltaHeight * speed * water * u_sedimentCapacityFactor, u_minSedimentCapacity);

        // If carrying more sediment than capacity, or if flowing uphill:
        if (sediment > sedimentCapacity || deltaHeight > 0.0) 
        {
            Deposit(sedimentCapacity, sediment, deltaHeight, cellOffsetX, cellOffsetY, dropletIndex);
        } 
        else
        {
            Erode(sedimentCapacity, sediment, deltaHeight, dropletIndex);
        }

        // Update droplet's speed and water content
        speed = sqrt(max(0.0, speed * speed + deltaHeight * u_gravity));
        water *= (1.0 - u_evaporateSpeed);
    }
}