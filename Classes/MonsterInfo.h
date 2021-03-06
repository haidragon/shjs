﻿//////////////
/****怪物****/
//////////////

static const int g_iMonsterFrame[] =
{
	60,
	60,
	60,
	60,
	60,
	40,
	60,
	18,
	40,
	60,
	60,
	60,
	60,
	60,
	60,
	60,
	60,
	60,
};
static const float g_fMonsterSpeed[] =
{
	0.80f,
	0.00f,
	1.50f,
	1.0f,
	1.2f,
	1.00f,
	2.50f,
	2.00f,
	0.00f,
	1.50f,
	0.50f,
	0.80f,
	0.00f,
	1.0f,
	0,
	0.6f,
	1.0f,
	2.0f,
};
static const int g_iMonsterHp[] =
{
	100,
	120,
	180,
	200,
	150,
	8000,
	100,
	100,
	12000,
	250,
	100,
	100,
	42000,
	60000,
	80000,
	3000,
	800,
	150000,
};
static const int g_iMonsterDamage[] =
{
	5,
	8,
	10,
	12,
	11,
	20,
	8,
	8,
	10,
	15,
	15,
	20,
	20,
	10,
	10,
	30,
	5,
	10,
};
static const float g_fMonsterWidth[] =
{
	40.0,
	40.0,
	40.0,
	40.0,
	40.0,
	50.0,
	65.0,
	60.0,
	80.0,
	40.0,
	40.0,
	40.0,
	40.0,
	80.0,
	190.0,
	40.0,
	30.0,
	40.0,
};
static const float g_fMonsterHeight[] =
{
	80.0,
	80.0,
	80.0,
	80.0,
	80.0,
	100.0,
	53.0,
	60.0,
	100.0,
	80.0,
	80.0,
	80.0,
	120,
	120,
	120,
	80,
	60,
	120,
};
static const int g_iMonsterAppearTime[] =
{
	65,
	0,
	0,
	65,
	65,
	65,
	0,
	0,
	0,
	65,
	0,
	65,
	0,
	0,
	0,
	65,
	0,
	0,
};
static const int g_iMonsterAttackTime[] =
{
	65,
	90,
	80,
	80,
	75,
	105,
	90,
	130,
	230,
	90,
	80,
	70,
	130,
	110,
	0,
	108,
	0,
	0,
};
static const int g_iMonsterHoldTime[] =
{
	25,
	25,
	25,
	25,
	25,
	0,
	25,
	25,
	0,
	25,
	25,
	25,
	0,
	0,
	0,
	10,
	0,
	0,
};
static const float g_fMonsterAttackDist[] =
{
	200,
	300,
	200,
	200,
	200,
	800,
	200,
	400,
	800,
	200,
	300,
	300,
	800,
	800,
	800,
	800,
	800,
	800,
};
static const int g_iMonsterBorn[] =
{
	4,
	0,
	4,
	4,
	4,
	7,
	0,
	0,
	0,
	4,
	0,
	0,
	4,
	0,
	0,
	3,
	0,
	0,
};
static const int g_iMonsterDeadGun[] =
{
	2,
	3,
	2,
	2,
	2,
	8,
	2,
	2,
	2,
	2,
	4,
	4,
	8,
	12,
	9,
	5,
	4,
	7,
};
static const int g_iMonsterRelife[] =
{
	0,
	0,
	0,
	6,
	6,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};
static const int g_iMonsterDeadSecond[] =
{
	0,
	0,
	0,
	9,
	9,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};
static const int g_iMonsterDeadKnife[] =
{
	3,
	4,
	3,
	3,
	3,
	8,
	3,
	3,
	3,
	3,
	4,
	4,
	9,
	12,
	9,
	6,
	4,
	7,
};
static const int g_iMonsterHold[] =
{
	5,
	5,
	5,
	5,
	5,
	6,
	4,
	4,
	-1,
	5,
	3,
	3,
	-1,
	-1,
	-1,
	4,
	-1,
	-1,
};
static const int g_iMonsterHoldSecond[] =
{
	0,
	0,
	0,
	10,
	10,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};
static const int g_iMonsterAttackSecond[] =
{
	0,
	0,
	0,
	8,
	8,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};
static const int g_iMonsterMoveSecond[] =
{
	0,
	0,
	0,
	7,
	7,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};

static const bool g_bMonsterCanDeadOnec[] =
{
	false,
	false,
	false,
	true,
	true,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
};
