#include <NTL/ZZ.h>
#include <NTL/lip.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZ_limbs.h>
#include <NTL/vec_ZZ_p.h>
#include <NTL/mat_ZZ_p.h>
#include <vector>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include "../hps-master/src/hps.h"

#include <chrono> 

using namespace std::chrono;
using namespace std;
using namespace NTL;

// typedef vector<ZZ_limb_t> ZZ_limbs;
typedef std::string ZZ_limbs;
// typedef const char* ZZ_limbs;



template <typename T>
class StringSerializer {
public :
   static void OpaqueToString(string &serializedString, T &obj) 
   {
      // serialize obj into an std::string
      boost::iostreams::back_insert_device<std::string> inserter(serializedString);
      boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
      boost::archive::binary_oarchive oa(s);

      oa << obj;

      // don't forget to flush the stream to finish writing into the buffer
      s.flush();
   }

   static void StringToOpaque(T &obj, string &serial_str) 
   {
      boost::iostreams::basic_array_source<char> device(serial_str.data(), serial_str.size());
      boost::iostreams::stream<boost::iostreams::basic_array_source<char> > t(device);
      boost::archive::binary_iarchive ia(t);
      ia >> obj;
   }
};



void printLimbs(vector<ZZ_limb_t> &l) {
    long size = l.size();
    for (int i =0; i< size; i++) {
        cout << l[i] << endl;
    }
}

void VecLimbToZZ_p(ZZ_p &zzp, vector<ZZ_limb_t> &arr) {
    long size = arr.size();    
    ZZ_limb_t limbs[size];
    for (int i =0; i< size; i++) {
        limbs[i] = arr[i];
    }
    ZZ a;
    ZZ_limb_t *const l = limbs;
    ZZ_limbs_set(a, l, size);
    zzp =  to_ZZ_p(a);    
}

void ZZ_pToVecLimb(vector<ZZ_limb_t> &limbs, ZZ_p &x) {
    ZZ zz = rep(x);
    long size = zz.size();
    limbs.reserve(size);
    const ZZ_limb_t *l = ZZ_limbs_get(zz);
    for (int i =0; i< size; i++) {
        limbs.push_back(l[i]);
    }
}

class OpaqueZZp {
   public:
        long size;
        ZZ_limb_t *l;

        template <class B>
        void serialize(B& buf) const {
            buf << size;
            for(int i =0; i<size; i++){
                buf << l[i];
            }
        }

        template <class B>
        void parse(B& buf) {
            buf >> size;
            l = new ZZ_limb_t[size];
            for(int i =0; i<size; i++){
                buf >> l[i];
            }
        }

        OpaqueZZp(ZZ_p &zzp) {
            ZZ zz = rep(zzp);
            size = zz.size();
            l = const_cast<ZZ_limb_t*>(ZZ_limbs_get(zz));
        }
        OpaqueZZp(){}
        void OpaqueZZpToString(std::string &ss) {
            
        }
        static void StringToZZp(ZZ_p &zzp, std::string &ss) {            
            OpaqueZZp ozzp2;
            hps::from_string<OpaqueZZp>(ss, ozzp2);
            ZZ a;
            ZZ_limbs_set(a, ozzp2.l, ozzp2.size);
            zzp =  to_ZZ_p(a);
        }

        static void ZZpToString(std::string &ss, ZZ_p &zzp) {
            OpaqueZZp ozzp2(zzp);
            hps::to_string(ozzp2, ss);
        }    
};

void LimbsToZZ_p(ZZ_p &zzp, ZZ_limbs &arr) {
    OpaqueZZp::StringToZZp(zzp, arr);
}

void ZZ_pToLimbs(ZZ_limbs &limbs, ZZ_p &x) {
    OpaqueZZp::ZZpToString(limbs, x);
}

int main() {
   const char* P = "52435875175126190479447740508185965837690552500527637822603658699938581184513";
   const char* A = "94606641538518525124626052516448957590052573267403774387184360108182625896033";
   const char* B = "12807130640567138200355688980399645868673927563246444718103192941204902114090";
   
   float tests = 10000000.0;

   string aString(A);


   ZZ p;
   p = to_ZZ(conv<ZZ>(P));
   ZZ_p::init(p);
   // cout << p << endl;

   ZZ ZZa, ZZb;
   ZZa = to_ZZ(conv<ZZ>(A));
   ZZb = to_ZZ(conv<ZZ>(B));

   ZZ_p ZZpa, ZZpb;

   ZZpa = to_ZZ_p(ZZa);
   ZZpb = to_ZZ_p(ZZb);

   // auto start = std::chrono::system_clock::now();
//    auto end = std::chrono::system_clock::now();
//    auto elapsed = end - start;
//    std::cout << elapsed.count() << '\n';

//    ZZ_limbs zl1;
//    ZZ_pToLimbs(zl1, ZZpa);
//    ZZ_p zzps;
//    LimbsToZZ_p(zzps, zl1);
//    cout << zl1 << endl;
//    cout << zzps << endl;

    std::string x;
    ZZ_p ZZpx;
    OpaqueZZp::ZZpToString(x, ZZpa);
    // cout << x << endl;
    // cout << "hello" << endl;
    OpaqueZZp::StringToZZp(ZZpx, x);
    cout << ZZpx << endl; 

   
   
   return 0;
}