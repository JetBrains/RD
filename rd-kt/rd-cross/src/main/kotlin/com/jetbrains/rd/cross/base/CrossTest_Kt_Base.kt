package com.jetbrains.rd.cross.base

import com.jetbrains.rd.cross.util.CombinatorLoggerFactory
import com.jetbrains.rd.cross.util.CrossTestsLoggerFactory
import com.jetbrains.rd.cross.util.logWithTime
import com.jetbrains.rd.framework.IProtocol
import com.jetbrains.rd.util.*
import com.jetbrains.rd.util.lifetime.Lifetime
import com.jetbrains.rd.util.log.ErrorAccumulatorLoggerFactory
import com.jetbrains.rd.util.reactive.IScheduler
import java.io.File

private const val SPINNING_TIMEOUT = 10_000L

@Suppress("ClassName")
abstract class CrossTest_Kt_Base {
    private val testName: String = this.javaClass.kotlin.simpleName!!

    private lateinit var outputFile: File

    protected lateinit var scheduler: IScheduler
    protected lateinit var protocol: IProtocol

    private val modelLifetimeDef = Lifetime.Eternal.createNested()
    private val socketLifetimeDef = Lifetime.Eternal.createNested()

    protected val modelLifetime = modelLifetimeDef.lifetime
    protected val socketLifetime = socketLifetimeDef.lifetime

    private fun before(args: Array<String>) {
        check(args.size == 1) {
            "Wrong number of arguments for $testName:${args.size}, expected 1. main([\"CrossTest_AllEntities_KtServer\"]) for example."
        }
        val outputFileName = args[0]
        outputFile = File(outputFileName)
        println("Test:$testName started, file=$outputFileName")
    }

    private fun after() {
        logWithTime("Spinning started")
        spinUntil(SPINNING_TIMEOUT) { false }
        logWithTime("Spinning finished")

        socketLifetimeDef.terminate()
        modelLifetimeDef.terminate()
    }

    fun run(args: Array<String>) {
        logWithTime("Test run")

        val buffer = mutableListOf<String>()

        Statics<ILoggerFactory>().use(CombinatorLoggerFactory(listOf(
            ConsoleLoggerFactory.apply {
                minLevelToLog = LogLevel.Trace
            },
            CrossTestsLoggerFactory(buffer),
            ErrorAccumulatorLoggerFactory
        ))) {
            try {
                before(args)
                start(args)
                after()
            } catch (e: Throwable) {
                e.printStackTrace()
                throw e
            } finally {
                if (::outputFile.isInitialized) {
                    outputFile.printWriter().buffered().use { out ->
                        buffer.forEach { line ->
                            out.write(line)
                            out.newLine()
                        }
                    }
                }
                ErrorAccumulatorLoggerFactory.throwAndClear()
            }
        }
    }

    fun queue(action: () -> Unit) {
        scheduler.queue {
            try {
                action()
            } catch (e: Throwable) {
                ErrorAccumulatorLoggerFactory.getLogger(testName).error { "Async error occurred" }
                e.printStackTrace()
            }
        }
    }

    abstract fun start(args: Array<String>)
}