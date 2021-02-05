## BedrockFormat
#### BedrockFormat is a library used to read and write Minecraft: Bedrock Edition worlds.
This library is able to parse worlds from Minecraft Bedrock 1.12.1+

---
<br>

The following code loads a subchunk located at 0, 0, 0 in the overworld and then retrieves the block at position 0, 0, 0 in that specific subchunk:
<pre lang="cpp">
World* minecraftWorld;
Result result = OpenWorld(worldPath, &world);
if(BF_FAILED(result)) {
    // An error occurred opening the world
}

// Loads the subchunk located at 0, 0, 0 in the overworld
Subchunk* subchunk;
result = LoadSubchunk(world, &subchunk, 0, 0, 0, OVERWORLD);
if(BF_FAILED(result)) {
    // An error occurred loading the subchunk
}

NbtTag* block = GetBlockAtSubchunkPosition(subchunk, 0, 0, 0);
if(block == NULL) {
    // An error occurred retrieving the block at position 0, 0, 0 in this subchunk
}

PrintNbtTag(block);

result = CloseWorld(minecraftWorld);
if(BF_FAILED(result)) {
    // An error occurred closing the world
}
</pre>

---
Copyright (c) 2021 Pathfinders<br>
All rights reserved.

