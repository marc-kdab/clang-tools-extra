// RUN: %check_clang_tidy %s qt-modernize-qsharedpointer-create %t

template <typename type>
class QSharedPointer {
public:
  QSharedPointer();
  QSharedPointer(type *ptr);
  QSharedPointer(const QSharedPointer<type> &t) {}
  QSharedPointer(QSharedPointer<type> &&t) {}
  ~QSharedPointer();
  type &operator*() { return *ptr; }
  type *operator->() { return ptr; }
  type *release();
  void reset();
  void reset(type *pt);
  QSharedPointer &operator=(QSharedPointer &&);
  template <typename T>
  QSharedPointer &operator=(QSharedPointer<T> &&);

  template <typename...Args>
  QSharedPointer create(Args &&...args);
  
private:
  type *ptr;
};

struct Base {
  Base();
  Base(int, int);
};

struct Derived : public Base {
  Derived();
  Derived(int, int);
};

struct APair {
  int a, b;
};

struct DPair {
  DPair() : a(0), b(0) {}
  DPair(int x, int y) : a(y), b(x) {}
  int a, b;
};

struct Empty {};

template <class T>
using QSharedPointer_ = QSharedPointer<T>;

void *operator new(__SIZE_TYPE__ Count, void *Ptr);

int g(QSharedPointer<int> P);

QSharedPointer<Base> getPointer() {
  return QSharedPointer<Base>(new Base);
  // CHECK-MESSAGES: :[[@LINE-1]]:10: warning: use create instead
  // CHECK-FIXES: return QSharedPointer<Base>::create();
}

void basic() {
  QSharedPointer<int> P1 = QSharedPointer<int>(new int());
  // CHECK-MESSAGES: :[[@LINE-1]]:28: warning: use create instead [qt-modernize-qsharedpointer-create]
  // CHECK-FIXES: QSharedPointer<int> P1 = QSharedPointer<int>::create();

  P1.reset(new int());
  // CHECK-MESSAGES: :[[@LINE-1]]:6: warning: use create instead [qt-modernize-qsharedpointer-create]
  // CHECK-FIXES: P1 = QSharedPointer<int>::create();

  P1 = QSharedPointer<int>(new int());
  // CHECK-MESSAGES: :[[@LINE-1]]:8: warning: use create instead [qt-modernize-qsharedpointer-create]
  // CHECK-FIXES: P1 = QSharedPointer<int>::create();

  // Without parenthesis.
  QSharedPointer<int> P2 = QSharedPointer<int>(new int);
  // CHECK-MESSAGES: :[[@LINE-1]]:28: warning: use create instead [qt-modernize-qsharedpointer-create]
  // CHECK-FIXES: QSharedPointer<int> P2 = QSharedPointer<int>::create();

  P2.reset(new int);
  // CHECK-MESSAGES: :[[@LINE-1]]:6: warning: use create instead [qt-modernize-qsharedpointer-create]
  // CHECK-FIXES: P2 = QSharedPointer<int>::create();

  P2 = QSharedPointer<int>(new int);
  // CHECK-MESSAGES: :[[@LINE-1]]:8: warning: use create instead [qt-modernize-qsharedpointer-create]
  // CHECK-FIXES: P2 = QSharedPointer<int>::create();

  // With auto.
  auto P3 = QSharedPointer<int>(new int());
  // CHECK-MESSAGES: :[[@LINE-1]]:13: warning: use create instead
  // CHECK-FIXES: auto P3 = QSharedPointer<int>::create();

  QSharedPointer<int> R(new int());

  // Create the QSharedPointer as a parameter to a function.
  int T = g(QSharedPointer<int>(new int()));
  // CHECK-MESSAGES: :[[@LINE-1]]:13: warning: use create instead
  // CHECK-FIXES: int T = g(QSharedPointer<int>::create());

  // Only replace if the type in the template is the same as the type returned
  // by the new operator.
  auto Pderived = QSharedPointer<Base>(new Derived());

  // OK to replace for reset and assign
  Pderived.reset(new Derived());
  // CHECK-MESSAGES: :[[@LINE-1]]:12: warning: use create instead
  // CHECK-FIXES: Pderived = QSharedPointer<Derived>::create();

  Pderived = QSharedPointer<Derived>(new Derived());
  // CHECK-MESSAGES: :[[@LINE-1]]:14: warning: use create instead
  // CHECK-FIXES: Pderived = QSharedPointer<Derived>::create();

  // FIXME: OK to replace if assigned to QSharedPointer<Base>
  Pderived = QSharedPointer<Base>(new Derived());

  // FIXME: OK to replace when auto is not used
  QSharedPointer<Base> PBase = QSharedPointer<Base>(new Derived());

  // The pointer is returned by the function, nothing to do.
  QSharedPointer<Base> RetPtr = getPointer();

  // This emulates move.
  QSharedPointer<int> Move = static_cast<QSharedPointer<int> &&>(P1);

  // Placement arguments should not be removed.
  int *PInt = new int;
  QSharedPointer<int> Placement = QSharedPointer<int>(new (PInt) int{3});
  Placement.reset(new (PInt) int{3});
  Placement = QSharedPointer<int>(new (PInt) int{3});
}

// Calling make_smart_ptr from within a member function of a type with a
// private or protected constructor would be ill-formed.
class Private {
private:
  Private(int z) {}

public:
  Private() {}
  void create() {
    auto callsPublic = QSharedPointer<Private>(new Private);
    // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: use create instead
    // CHECK-FIXES: auto callsPublic = QSharedPointer<Private>::create();
    auto ptr = QSharedPointer<Private>(new Private(42));
    ptr.reset(new Private(42));
    ptr = QSharedPointer<Private>(new Private(42));
  }

  virtual ~Private();
};

class Protected {
protected:
  Protected() {}

public:
  Protected(int, int) {}
  void create() {
    auto callsPublic = QSharedPointer<Protected>(new Protected(1, 2));
    // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: use create instead
    // CHECK-FIXES: auto callsPublic = QSharedPointer<Protected>::create(1, 2);
    auto ptr = QSharedPointer<Protected>(new Protected);
    ptr.reset(new Protected);
    ptr = QSharedPointer<Protected>(new Protected);
  }
};

void initialization(int T, Base b) {
  // Test different kinds of initialization of the pointee.

  // Direct initialization with parenthesis.
  QSharedPointer<DPair> PDir1 = QSharedPointer<DPair>(new DPair(1, T));
  // CHECK-MESSAGES: :[[@LINE-1]]:33: warning: use create instead
  // CHECK-FIXES: QSharedPointer<DPair> PDir1 = QSharedPointer<DPair>::create(1, T);
  PDir1.reset(new DPair(1, T));
  // CHECK-MESSAGES: :[[@LINE-1]]:9: warning: use create instead
  // CHECK-FIXES: PDir1 = QSharedPointer<DPair>::create(1, T);

  // Direct initialization with braces.
  QSharedPointer<DPair> PDir2 = QSharedPointer<DPair>(new DPair{2, T});
  // CHECK-MESSAGES: :[[@LINE-1]]:33: warning: use create instead
  // CHECK-FIXES: QSharedPointer<DPair> PDir2 = QSharedPointer<DPair>::create(2, T);
  PDir2.reset(new DPair{2, T});
  // CHECK-MESSAGES: :[[@LINE-1]]:9: warning: use create instead
  // CHECK-FIXES: PDir2 = QSharedPointer<DPair>::create(2, T);

  // Aggregate initialization.
  QSharedPointer<APair> PAggr = QSharedPointer<APair>(new APair{T, 1});
  // CHECK-MESSAGES: :[[@LINE-1]]:33: warning: use create instead
  // CHECK-FIXES: QSharedPointer<APair> PAggr = QSharedPointer<APair>::create(APair{T, 1});
  PAggr.reset(new APair{T, 1});
  // CHECK-MESSAGES: :[[@LINE-1]]:9: warning: use create instead
  // CHECK-FIXES: QSharedPointer<APair>::create(APair{T, 1});

  // Test different kinds of initialization of the pointee, when the QSharedPointer
  // is initialized with braces.

  // Direct initialization with parenthesis.
  QSharedPointer<DPair> PDir3 = QSharedPointer<DPair>{new DPair(3, T)};
  // CHECK-MESSAGES: :[[@LINE-1]]:33: warning: use create instead
  // CHECK-FIXES: QSharedPointer<DPair> PDir3 = QSharedPointer<DPair>::create(3, T);

  // Direct initialization with braces.
  QSharedPointer<DPair> PDir4 = QSharedPointer<DPair>{new DPair{4, T}};
  // CHECK-MESSAGES: :[[@LINE-1]]:33: warning: use create instead
  // CHECK-FIXES: QSharedPointer<DPair> PDir4 = QSharedPointer<DPair>::create(4, T);

  // Aggregate initialization.
  QSharedPointer<APair> PAggr2 = QSharedPointer<APair>{new APair{T, 2}};
  // CHECK-MESSAGES: :[[@LINE-1]]:34: warning: use create instead
  // CHECK-FIXES: QSharedPointer<APair> PAggr2 = QSharedPointer<APair>::create(APair{T, 2});

  // Direct initialization with parenthesis, without arguments.
  QSharedPointer<DPair> PDir5 = QSharedPointer<DPair>(new DPair());
  // CHECK-MESSAGES: :[[@LINE-1]]:33: warning: use create instead
  // CHECK-FIXES: QSharedPointer<DPair> PDir5 = QSharedPointer<DPair>::create();

  // Direct initialization with braces, without arguments.
  QSharedPointer<DPair> PDir6 = QSharedPointer<DPair>(new DPair{});
  // CHECK-MESSAGES: :[[@LINE-1]]:33: warning: use create instead
  // CHECK-FIXES: QSharedPointer<DPair> PDir6 = QSharedPointer<DPair>::create();

  // Aggregate initialization without arguments.
  QSharedPointer<Empty> PEmpty = QSharedPointer<Empty>(new Empty{});
  // CHECK-MESSAGES: :[[@LINE-1]]:34: warning: use create instead
  // CHECK-FIXES: QSharedPointer<Empty> PEmpty = QSharedPointer<Empty>::create(Empty{});
}

void aliases() {
  typedef QSharedPointer<int> IntPtr;
  IntPtr Typedef = IntPtr(new int);
  // CHECK-MESSAGES: :[[@LINE-1]]:20: warning: use create instead
  // CHECK-FIXES: IntPtr Typedef = IntPtr::create();

  // We use 'bool' instead of '_Bool'.
  typedef QSharedPointer<bool> BoolPtr;
  BoolPtr BoolType = BoolPtr(new bool);
  // CHECK-MESSAGES: :[[@LINE-1]]:22: warning: use create instead
  // CHECK-FIXES: BoolPtr BoolType = BoolPtr::create();

  // We use 'Base' instead of 'struct Base'.
  typedef QSharedPointer<Base> BasePtr;
  BasePtr StructType = BasePtr(new Base);
  // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: use create instead
  // CHECK-FIXES: BasePtr StructType = BasePtr::create();

#define PTR QSharedPointer<int>
  QSharedPointer<int> Macro = PTR(new int);
  // CHECK-MESSAGES: :[[@LINE-1]]:31: warning: use create instead
  // -CHECK-FIXES: QSharedPointer<int> Macro = PTR::create();
#undef PTR

  QSharedPointer<int> Using = QSharedPointer_<int>(new int);
  // CHECK-MESSAGES: :[[@LINE-1]]:31: warning: use create instead
  // CHECK-FIXES: QSharedPointer<int> Using = QSharedPointer_<int>::create();
}

void whitespaces() {
  // clang-format off
  auto Space = QSharedPointer <int>(new int());
  // CHECK-MESSAGES: :[[@LINE-1]]:16: warning: use create instead
  // CHECK-FIXES: auto Space = QSharedPointer <int>::create();

  auto Spaces = QSharedPointer  <int>(new int());
  // CHECK-MESSAGES: :[[@LINE-1]]:17: warning: use create instead
  // CHECK-FIXES: auto Spaces = QSharedPointer  <int>::create();
  // clang-format on
}

void nesting() {
  auto Nest = QSharedPointer<QSharedPointer<int>>(new QSharedPointer<int>(new int));
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: use create instead
  // CHECK-FIXES: auto Nest = QSharedPointer<QSharedPointer<int>>::create(new int);
  Nest.reset(new QSharedPointer<int>(new int));
  // CHECK-MESSAGES: :[[@LINE-1]]:8: warning: use create instead
  // CHECK-FIXES: Nest = QSharedPointer<QSharedPointer<int>>::create(new int);
  Nest->reset(new int);
  // CHECK-MESSAGES: :[[@LINE-1]]:9: warning: use create instead
  // CHECK-FIXES: *Nest = QSharedPointer<int>::create();
}

void reset() {
  QSharedPointer<int> P;
  P.reset();
  P.reset(nullptr);
  P.reset(new int());
  // CHECK-MESSAGES: :[[@LINE-1]]:5: warning: use create instead
  // CHECK-FIXES: P = QSharedPointer<int>::create();

  auto Q = &P;
  Q->reset(new int());
  // CHECK-MESSAGES: :[[@LINE-1]]:6: warning: use create instead
  // CHECK-FIXES: *Q = QSharedPointer<int>::create();
}
