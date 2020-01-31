#import "iosTestRunner.h"

#include "ios_test_runner.hpp"

#include <string>

@interface IosTestRunner ()

@property (nullable) TestRunner* runner;

@property (copy, nullable) NSString *resultPath;

@property BOOL testStatus;

@end

@implementation IosTestRunner

-(instancetype)init
{
    self = [super init];
    if (self) {
        self.testStatus = NO;
        self.runner = new TestRunner();
        NSString *path = nil;
        NSError *error;
        NSFileManager *fileManager = [NSFileManager defaultManager];
        NSString *bundleRoot = [[NSBundle mainBundle] bundlePath];
        NSArray *bundleContents = [fileManager contentsOfDirectoryAtPath: bundleRoot error: &error];
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDir = [paths objectAtIndex: 0];
        
        for (uint32_t i = 0; i < bundleContents.count; i++) {
            NSString *dirName = [bundleContents objectAtIndex: i];
            if ([dirName isEqualToString:@"test-data"]) {
                NSString *destinationPath = [documentsDir stringByAppendingPathComponent: dirName];
                //Using NSFileManager we can perform many file system operations.
                BOOL success = [fileManager fileExistsAtPath: destinationPath];
                if (success) {
                    success = [fileManager removeItemAtPath:destinationPath error:NULL];
                }
                break;
            }
        }
        
        for (uint32_t i = 0; i < bundleContents.count; i++) {
            NSString *dirName = [bundleContents objectAtIndex: i];
            if ([dirName isEqualToString:@"test-data"]) {
                NSString *destinationPath = [documentsDir stringByAppendingPathComponent: dirName];
                BOOL success = [fileManager fileExistsAtPath: destinationPath];
                if (!success) {
                    NSString *copyDirPath  = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent: dirName];

                    success = [fileManager copyItemAtPath: copyDirPath toPath: destinationPath error: &error];
              
                    if (!success){
                        NSAssert1(0, @"Failed to copy file '%@'.", [error localizedDescription]);
                        NSLog(@"Failed to copy %@ file, error %@", dirName, [error localizedDescription]);
                    }
                    else {
                        path = destinationPath;
                        NSLog(@"File copied %@ OK", dirName);
                    }
                }
                else {
                    NSLog(@"File exits %@, skip copy", dirName);
                }
                break;
            }
        }
        self.testStatus = self.runner->startTest() ? YES : NO;
        self.resultPath =  [path stringByAppendingPathComponent:@"/results.xml"];

        BOOL fileFound = [fileManager fileExistsAtPath: self.resultPath];
        if (fileFound == NO) {
            NSLog(@"Test result file '%@' doese not exit ", self.resultPath);
            self.testStatus = NO;
        }

        delete self.runner;
        self.runner = nullptr;
    }
    return self;
}

- (NSString*) getResultPath {
    return self.resultPath;
}

- (BOOL) getTestStatus {
    return self.testStatus;
}
@end
