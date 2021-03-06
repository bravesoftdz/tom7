
var tiles = 
[
 { id: 0, isbg: false, frames: [] },
 { id: 1, isbg: false, frames: ['floor', 1] },
 { id: 2, isbg: true,  frames: ['solidteal', 1] },
 { id: 3, isbg: true,  frames: ['solidtealv1', 1] },
 { id: 4, isbg: true,  frames: ['solidtealv2', 1] },
 { id: 5, isbg: true,  frames: ['solidtealv3', 1] },
 { id: 6, isbg: true,  frames: ['vip1', 1] },
 { id: 7, isbg: true,  frames: ['vip2', 1] },
 { id: 8, isbg: true,  frames: ['cubetop1', 1] },
 { id: 9, isbg: true,  frames: ['cubetop2', 1] },
 { id: 10, isbg: true,  frames: ['cubetop3', 1] },
 { id: 11, isbg: true,  frames: ['cubetop4', 1] },
 { id: 12, isbg: false,  frames: ['cubebot1', 1] },
 { id: 13, isbg: false,  frames: ['cubebot2', 1] },
 { id: 14, isbg: false,  frames: ['cubebot3', 1] },
 { id: 15, isbg: false,  frames: ['cubebot4', 1] },
 { id: 16, isbg: true,  frames: ['restroomsdown', 1] },
 { id: 17, isbg: true,  frames: ['restroomsleft', 1] },
 { id: 18, isbg: true,  frames: ['restrooms2', 1] },
 { id: 19, isbg: true,  frames: ['bathroomtile1', 1] },
 { id: 20, isbg: true,  frames: ['bathroomtile2', 1] },
 { id: 21, isbg: true,  frames: ['bathroomtile3', 1] },
 { id: 22, isbg: true,  frames: ['mirrortl', 1] },
 { id: 23, isbg: true,  frames: ['mirrortr', 1] },
 { id: 24, isbg: true,  frames: ['mirrorbl', 1] },
 { id: 25, isbg: true,  frames: ['mirrorbr', 1] },
 { id: 26, isbg: false,  frames: ['sinkl', 1] },
 { id: 27, isbg: false,  frames: ['sinkr', 1] },
 { id: 28, isbg: false,  frames: ['pipe', 1] },
 { id: 29, isbg: true,  frames: ['stall1', 1] },
 { id: 30, isbg: true,  frames: ['stall2', 1] },
 { id: 31, isbg: true,  frames: ['stall3', 1] },
 { id: 32, isbg: true,  frames: ['stall4', 1] },
 { id: 33, isbg: true,  frames: ['stall5', 1] },
 { id: 34, isbg: true,  frames: ['stall6', 1] },
 { id: 35, isbg: true,  frames: ['stall7', 1] },
 { id: 36, isbg: true,  frames: ['stall8', 1] },
 { id: 37, isbg: true,  frames: ['stall9', 1] },
 { id: 38, isbg: true,  frames: ['stall10', 1] },
 { id: 39, isbg: true,  frames: ['stallsolid', 1] },
 { id: 40, isbg: true,  frames: ['ooo1', 1] },
 { id: 41, isbg: true,  frames: ['ooo2', 1] },
 { id: 42, isbg: true,  frames: ['ooo3', 1] },
 { id: 43, isbg: true,  frames: ['ooo4', 1] },
 { id: 44, isbg: true,  frames: ['ooo5', 1] },
 { id: 45, isbg: true,  frames: ['ooo6', 1] },
 { id: 46, isbg: true,  frames: ['ooo7', 1] },
 { id: 47, isbg: true,  frames: ['ooo8', 1] },
 { id: 48, isbg: true,  frames: ['ooo9', 1] },
 { id: 49, isbg: true,  frames: ['ooo10', 1] },
 { id: 50, isbg: true,  frames: ['ooo11', 1] },
 { id: 51, isbg: true,  frames: ['dirt1', 1] },
 { id: 52, isbg: true, frames: ['club1', 1] },
 { id: 53, isbg: true, frames: ['club2', 1] },
 { id: 54, isbg: true, frames: ['club3', 1] },
 { id: 55, isbg: true, frames: ['club4', 1] },
 { id: 56, isbg: true, frames: ['club5', 1] },
 { id: 57, isbg: true, frames: ['club6', 1] },
 { id: 58, isbg: true, frames: ['club7', 1] },
 { id: 59, isbg: true, frames: ['club8', 1] },
 { id: 60, isbg: true, frames: ['club9', 1] },
 { id: 61, isbg: true, frames: ['club10', 1] },
 { id: 62, isbg: true, frames: ['club11', 1] },
 { id: 63, isbg: true, frames: ['club12', 1] },
 { id: 64, isbg: true, frames: ['club13', 1] },
 { id: 65, isbg: true, frames: ['club14', 1] },
 { id: 66, isbg: true, frames: ['club15', 1] },
 { id: 67, isbg: true, frames: ['club16', 1] },
 { id: 68, isbg: true, frames: ['club17', 1] },
 { id: 69, isbg: true, frames: ['club18', 1] },
 { id: 70, isbg: true, frames: ['stars1', 1] },
 { id: 71, isbg: true, frames: ['stars2', 1] },
 { id: 72, isbg: true, frames: ['stars3', 1] },
 { id: 73, isbg: true, frames: ['stars4', 1] },
 { id: 74, isbg: false, frames: ['dirtwall', 1] },
 { id: 75, isbg: false, frames: ['dirtwalltop', 1] },
 { id: 76, isbg: false, deadly: true, frames: ['spikeup1', 1] },
 { id: 77, isbg: false, deadly: true, frames: ['spikeup2', 1] },
 { id: 78, isbg: false, deadly: true, frames: ['spikeup3', 1] },
 { id: 79, isbg: false, deadly: true, frames: ['spikedown1', 1] },
 { id: 80, isbg: false, deadly: true, frames: ['spikedown2', 1] },
 { id: 81, isbg: false, deadly: true, frames: ['spikedown3', 1] },
 { id: 82, isbg: false, frames: ['rock1', 1] },
 { id: 83, isbg: true, frames: ['tealblackdiag', 1]},
 { id: 84, isbg: false, frames: ['glass', 1] },
 { id: 85, isbg: true, frames: ['glasstable1', 1] },
 { id: 86, isbg: true, frames: ['glasstable2', 1] },
 // duplicate: 32,0: 436e5c0116
 // duplicate: 48,0: 436e5c0116
 { id: 87, isbg: true, frames: ['glasstable3', 1] },
 // duplicate: 80,0: 436e5c0116
 { id: 88, isbg: true, frames: ['glasstable4', 1] },
 { id: 89, isbg: false, frames: ['glasstable5', 1] },
 { id: 90, isbg: false, frames: ['glasstable6', 1] },
 { id: 91, isbg: false, frames: ['glasstable7', 1] },
 { id: 92, isbg: false, frames: ['glasstable8', 1] },
 { id: 93, isbg: false, frames: ['glasstable9', 1] },
 { id: 94, isbg: false, frames: ['glasstable10', 1] },
 { id: 95, isbg: false, frames: ['glasstable11', 1] },
 { id: 96, isbg: false, frames: ['glasstable12', 1] },
 { id: 97, isbg: false, frames: ['glasstable13', 1] },
 { id: 98, isbg: false, frames: ['glasstable14', 1] },
 { id: 99, isbg: false, frames: ['glasstable15', 1] },
 // duplicate: 64,32: 436e5c0116
 { id: 100, isbg: false, frames: ['glasstable16', 1] },
 { id: 101, isbg: false, frames: ['glasstable17', 1] },
 { id: 102, isbg: true, frames: ['wallpaper1', 1] },
 { id: 103, isbg: true, frames: ['wallpaper2', 1] },
 { id: 104, isbg: true, frames: ['wallpaper3', 1] },
 { id: 105, isbg: true, frames: ['wallpaper4', 1] },
 { id: 106, isbg: true, frames: ['wallpaper5', 1] },
 { id: 107, isbg: true, frames: ['wallpaper6', 1] },
 { id: 108, isbg: true, frames: ['wallpaper7', 1] },
 { id: 109, isbg: true, frames: ['wallpaper8', 1] },
 { id: 110, isbg: true, frames: ['wallpaper9', 1] },
 { id: 111, isbg: true, frames: ['brokenteal1', 1] },
 { id: 112, isbg: true, frames: ['brokenteal2', 1] }
 ];
