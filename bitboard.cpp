#include <iostream>
#include <iomanip>
#include <cassert>
#include "bitboard.h"

using namespace std;

SQ_Info sq_info[BBI_MAX];
unsigned int lut[65536];

int popcnt(U64 x)
{
    return lut[x >> 48] + lut[(x >> 32) & 0xFFFF]
         + lut[(x >> 16) & 0xFFFF] + lut[x & 0xFFFF];
}

int bitscan(U64 x)
{
    return btscn64[((x & (EMPTY - x)) * debruijn) >> 58];
}

void init_bitboards()
{
    for (int i = 0; i < 65536; i++)
    {
        int cnt = 0;
        for (int j = i; j; j &= j - 1) cnt++;
        lut[i] = cnt;
        //cout << i << " - " << cnt << "\n";
    }

    for (int y = 0; y < N; y++)
    {
        for (int x = 0; x < N; x++)
        {
            SQ sq = make_sq(x, y);
            sq_info[sq] = SQ_Info(x, y);

            //cout << (char)('A' + x) << (char)('1' + y) << " - " << sq << "\n";
        }
    }
}

void test_bitboards()
{
    // Testing shifts
    {
        BB bb = BB(bbi_mask0, bbi_mask1);
        assert(bb.lshift() == BB(0xff1fe3fc7f8ff000, 0x00000ff1fe3fc7f8));
        assert(bb.rshift() == BB(0xfe3fc7f8ff1fe000, 0x00001fe3fc7f8ff1));
        assert(bb.ushift() == BB(0xff3fe7fcff800000, 0x00001ff3fe7fcff9));
        assert(bb.dshift() == BB(0xff3fe7fcff9ff000, 0x00000003fe7fcff9));
    }

    // Testing bitscan
    {
        assert(BB().set("E4"_sq).bitscan() == "E4"_sq);
        assert(BB().set("D7"_sq).bitscan() == "D7"_sq);
        assert(BB().set("A1"_sq).bitscan() == "A1"_sq);
        assert(BB().set("I9"_sq).bitscan() == "I9"_sq);
    }
}

BB::BB(U64 p1, U64 p2)
{
    part[0] = p1;
    part[1] = p2;
}

void BB::clear()
{
    part[0] = part[1] = EMPTY;
}

BB & BB::set(SQ sq)
{
    int j = sq >> 6;
    int v = sq & 63;
    part[j] |= BIT << v;
    return *this;
}

BB & BB::reset(SQ sq)
{
    int j = sq >> 6;
    int v = sq & 63;
    part[j] &= ~(BIT << v);
    return *this;
}

bool BB::test(SQ sq) const
{
    int j = sq >> 6;
    int v = sq & 63;
    return (BIT << v) & part[j];
}

BB BB::operator & (const BB & bb) const
{
    return BB(part[0] & bb.part[0], part[1] & bb.part[1]);
}

BB BB::operator | (const BB & bb) const
{
    return BB(part[0] | bb.part[0], part[1] | bb.part[1]);
}

BB BB::operator ^ (const BB & bb) const
{
    return BB(part[0] ^ bb.part[0], part[1] ^ bb.part[1]);
}

BB & BB::operator &= (const BB & bb)
{
    part[0] &= bb.part[0];
    part[1] &= bb.part[1];
    return *this;
}

BB & BB::operator |= (const BB & bb)
{
    part[0] |= bb.part[0];
    part[1] |= bb.part[1];
    return *this;
}

BB & BB::operator ^= (const BB & bb)
{
    part[0] ^= bb.part[0];
    part[1] ^= bb.part[1];
    return *this;
}

BB BB::operator ~ ()
{
    return BB(~part[0] & bbi_mask0, ~part[1] & bbi_mask1);
}

bool BB::operator == (const BB & bb) const
{
    return (part[0] & bbi_mask0) == (bb.part[0] & bbi_mask0)
        && (part[1] & bbi_mask1) == (bb.part[1] & bbi_mask1);
}

bool BB::operator != (const BB & bb) const
{
    return (part[0] & bbi_mask0) != (bb.part[0] & bbi_mask0)
        || (part[1] & bbi_mask1) != (bb.part[1] & bbi_mask1);
}

BB BB::lshift() const
{
    U64 rem = part[1] << 63;
    U64 p0 = part[0] >> 1;
    U64 p1 = part[1] >> 1;
    return BB(p0 & bbi_mask0 | rem, p1 & bbi_mask1);
}

BB BB::rshift() const
{
    U64 rem = part[0] >> 63;
    U64 p0 = part[0] << 1;
    U64 p1 = part[1] << 1;
    return BB(p0 & bbi_mask0, p1 & bbi_mask1 | rem);
}

BB BB::ushift() const
{
    U64 rem = part[0] >> (64 - 11);
    U64 p0 = part[0] << 11;
    U64 p1 = part[1] << 11;
    return BB(p0 & bbi_mask0, p1 & bbi_mask1 | rem);
}

BB BB::dshift() const
{
    U64 rem = part[1] << (64 - 11);
    U64 p0 = part[0] >> 11;
    U64 p1 = part[1] >> 11;
    return BB(p0 & bbi_mask0 | rem, p1 & bbi_mask1);
}

BB BB::lsb() const
{
    U64 p0 = part[0] & (EMPTY - part[0]);
    U64 p1 = p0 ? EMPTY : part[1] & (EMPTY - part[1]);
    return BB(p0, p1);
}

BB BB::get_group() const
{
    BB group = lsb();
    BB last = EMPTY;

    while (group != last)
    {
        last = group;
        group |= group.lshift();
        group &= *this;
        group |= group.dshift();
        group &= *this;
        group |= group.rshift();
        group &= *this;
        group |= group.ushift();
        group &= *this;
    }
    return group;
}

BB BB::expand() const
{
    BB libs;
    libs |= this->lshift();
    libs |= this->dshift();
    libs |= this->rshift();
    libs |= this->ushift();
    return libs;
}

int BB::popcnt() const
{
    ASSERT(!(part[0] & ~bbi_mask0), print64(part[0]));
    ASSERT(!(part[1] & ~bbi_mask1), print64(part[1]));

    return ::popcnt(part[0]) + ::popcnt(part[1]);
}

int BB::bitscan() const
{
    ASSERT(!(part[0] & ~bbi_mask0), print64(part[0]));
    ASSERT(!(part[1] & ~bbi_mask1), print64(part[1]));

    return part[0]
        ? ::bitscan(part[0])
        : ::bitscan(part[1]) + 64;
}

BB::operator bool() const
{
    return part[0] || part[1];
}

void BB::print() const
{
    cout << hex;
    for (int y = N - 1; y >= 0; y--)
    {
        cout << setfill(' ') << setw(3) << (y + 1) << " " << setfill('0');
        for (int x = 0; x < N; x++)
        {
            SQ sq = make_sq(x, y);
            if (test(sq)) cout << "xx";
            else cout << (char)250 << (char)250;

            cout << " ";
        }
        cout << "\n";
    }
    cout << "     ";
    for (int x = 0; x < N; x++)
        cout << (char)('A' + x) << "  ";
    cout << "  { " << setw(16) << part[0] << " " << setw(16) << part[1] << " }\n";
    cout << dec << setfill(' ') << "\n\n";
}
