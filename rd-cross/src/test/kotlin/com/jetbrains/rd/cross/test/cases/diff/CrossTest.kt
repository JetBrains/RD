package com.jetbrains.rd.cross.test.cases.diff

import org.junit.Test
import java.io.File
import java.nio.file.Paths

class CrossTest {
    private fun assertEqualFiles(file1: File, file2: File) {
        val t1 = file1.readText(Charsets.UTF_8)
        val t2 = file2.readText(Charsets.UTF_8)
        if (t1 == t2) {
            println("The files $file1 and $file2 are same!")
        } else {
            error {
                """
                    |The files $file1 and $file2 differ!
                    |$file1:
                    |$t1
                    |$file2:
                    |$t2
                """.trimMargin()
            }
        }
    }

    @Test
    fun testAll() {
        val rootFolder = File(Paths.get("").toAbsolutePath().toString()).parent
        println("rootFolder=$rootFolder")
        val goldFolder = File(rootFolder, "buildSrc/src/main/resources/gold")
        val tmpFolder = File(rootFolder, "build/src/main/resources/tmp")
        assert(tmpFolder.exists()) { "Tmp directory($tmpFolder) was not created" }
        goldFolder.listFiles()!!.forEach {
            println("Gold file=${it.name}")
            val candidate = File(tmpFolder, it.nameWithoutExtension + ".tmp")
            assert(candidate.exists()) { "File $candidate doesn't exist" }
            assertEqualFiles(it, candidate)
        }

        tmpFolder.listFiles()!!.forEach {
            println("Tmp file=${it.name}")
            val candidate = File(goldFolder, it.nameWithoutExtension + ".gold")
            assert(candidate.exists()) { "Extra tmp file=$it" }
        }
    }
}