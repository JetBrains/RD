package com.jetbrains.rd.util

import com.jetbrains.rd.util.threading.SpinWait
import java.io.PrintWriter
import java.io.StringWriter
import java.util.Date
import java.util.concurrent.CancellationException
import java.util.concurrent.ExecutionException
import java.util.concurrent.TimeoutException
import java.util.concurrent.atomic.AtomicInteger
import java.util.concurrent.atomic.AtomicReference
import kotlin.io.use
import kotlin.reflect.KClass

actual typealias ExecutionException = ExecutionException

actual fun currentThreadName() : String = Thread.currentThread().run { "$id:$name"}

actual class AtomicReference<T> actual constructor(initial: T) {
    private val impl = AtomicReference(initial)
    actual fun get(): T = impl.get()
    actual fun getAndUpdate(f: (T) -> T): T {
        var prev: T
        var next: T
        do {
            prev = impl.get()
            next = f(prev)
        } while (!impl.compareAndSet(prev, next))
        return prev
    }
    fun getAndSet(new: T): T = impl.getAndSet(new)
}

actual typealias CancellationException = CancellationException
actual typealias TimeoutException = TimeoutException

actual typealias ThreadLocal<T> = java.lang.ThreadLocal<T>

actual fun <T> threadLocalWithInitial(initial: () -> T) : ThreadLocal<T> = ThreadLocal.withInitial(initial)

actual val eol : String = System.lineSeparator()

@Suppress("PLATFORM_CLASS_MAPPED_TO_KOTLIN")
actual object Sync {
    actual inline fun <R: Any?> lock(obj: Any, acton: () -> R) = synchronized(obj, acton)
    actual fun notifyAll(obj: Any) = (obj as Object).notifyAll()
    actual fun notify(obj: Any) = (obj as Object).notify()
    actual fun wait(obj: Any) = (obj as Object).wait()
    actual fun wait(obj: Any, timeout: Long) = (obj as Object).wait(timeout)
}

actual fun<K,V> concurrentMapOf() : MutableMap<K,V> = ConcurrentHashMap()

actual typealias Closeable = java.io.Closeable
actual inline fun <T : Closeable?, R> T.use(block:(T) -> R) : R = use(block)

actual fun Throwable.getThrowableText(): String = StringWriter().apply { printStackTrace(PrintWriter(this)) }.toString()

actual fun qualifiedName(kclass: KClass<*>) : String = kclass.qualifiedName?:"<anonymous>"

actual fun measureTimeMillis(block: () -> Unit): Long = kotlin.system.measureTimeMillis(block)

//special jvm classes
actual typealias URI = java.net.URI

actual typealias Date = Date

actual typealias UUID = java.util.UUID

actual typealias AtomicInteger = AtomicInteger

actual typealias Queue<E> = java.util.concurrent.LinkedBlockingQueue<E>
actual typealias ConcurrentHashMap<K, V> = java.util.concurrent.ConcurrentHashMap<K, V>

actual fun printlnError(msg: String) = System.err.println(msg)

actual fun assert(value: Boolean) = kotlin.assert(value)

actual inline fun assert(value: Boolean, lazyMessage: () -> Any)  = kotlin.assert(value, lazyMessage)

actual inline fun spinUntil(condition: () -> Boolean) = SpinWait.spinUntil(condition)
actual inline fun spinUntil(timeoutMs: Long, condition: () -> Boolean) = SpinWait.spinUntil(timeoutMs, condition)

actual typealias EnumSet<T> = java.util.EnumSet<T>
actual inline fun <reified T : Enum<T>> enumSetOf(values: Set<T>) : EnumSet<T> = EnumSet.noneOf(T::class.java).apply { addAll(values) }
actual fun <T: Enum<T>> EnumSet<T>.values() : Set<T> = this

actual typealias Runnable = java.lang.Runnable
actual typealias Callable<T> = java.util.concurrent.Callable<T>

actual typealias CopyOnWriteArrayList<T> = java.util.concurrent.CopyOnWriteArrayList<T>